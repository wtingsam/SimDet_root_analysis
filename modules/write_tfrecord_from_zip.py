import numpy as np
from numpy import *
from glob import glob
import tensorflow as tf
import sys
import os
import struct
import zipfile
import matplotlib.pyplot as plt
LABEL_NAMES = ["p_t", "p_z",
               "entry_x", "entry_y", "entry_z"]
COLOR_NAMES = ["dt", "q",
               "xe0", "ye0", "xe1", "ye1"]
max_pixels = 128
max_colors = 6
max_labels = len(LABEL_NAMES)
DEBUG = False

def _decode(data,offset):
    """
    data: binary file for whole file
    offset: bytes size offset for each event
    ---------
    return a list of labels[max_labels]
    and images[max_pixels,max_pixels,max_colors] 
    """
    images = []
    labels = []
    header_size = 2+2
    label_size = 4*max_labels
    # Read header
    iev   = struct.unpack('h',data[0+offset:2+offset])[0]
    nhits = struct.unpack('h',data[2+offset:4+offset])[0]
    # Loop over lables
    for j in range(max_labels):
        from_j = header_size + j*4 + offset
        to_j   = header_size + (j+1)*4 + offset
        # To fix a bug in transforming coordinate system
        sign = 1
        if j == 3:
            sign = -1
        # Append labels
        labels.append(struct.unpack('f',data[from_j:to_j])[0]*sign)
        # Read images
    # Loop over colors
    for j in range(max_colors):
        img_tmp = []
        # Loop over pixels * pixels
        for ip in range(max_pixels*max_pixels):
            from_ip = header_size + label_size + 4*(ip)   + offset + 4*max_pixels**2*j
            to_ip   = header_size + label_size + 4*(ip+1) + offset + 4*max_pixels**2*j
            img_pix = struct.unpack('f',data[from_ip:to_ip])[0]
            img_tmp.append(img_pix)
        images.append([img_tmp])
    return images, np.asarray(labels)

def write_array_to_tfrecord(array, labels, filename, options=None):
    # Open TFRecords file, ensure we use gzip compression
    writer = tf.python_io.TFRecordWriter(filename, options=options)
    print(type(array))
    print(array.shape)
    # Write all the images to a file
    for lbl, img in zip(labels, array):
        # Create the feature dictionary and enter the image
        image_as_bytes = tf.train.BytesList(value=[tf.compat.as_bytes(img.tostring())])
        feature = {'image':  tf.train.Feature(bytes_list=image_as_bytes)}
        # Create anentry for each label
        for a_lab, name_lab in zip(lbl, LABEL_NAMES):
            label_as_float = tf.train.FloatList(value=[a_lab])
            feature[name_lab] = tf.train.Feature(float_list=label_as_float)
        # Create an example protocol buffer
        example = tf.train.Example(features=tf.train.Features(feature=feature))
        # Serialize to string and write on the file
        writer.write(example.SerializeToString())
    # Close the writer and flush the buffer
    writer.close()

if __name__ == '__main__':
    file_path = sys.argv[1] # Full path assumption
    output_dir = sys.argv[2]
    max_pixels = int(sys.argv[3])
    filename = os.path.split(file_path)[1]
    output_filename = filename[:-4] + "_pixelized.tfrecord"
    output_path = output_dir + "/" + output_filename
    print("File name   :", file_path)
    print("Output name :", output_path)
    zf = zipfile.ZipFile(file_path)
    chunk_bytes = 2 + 2 + 4*max_labels + max_pixels*max_pixels*4*max_colors
    for filename in zf.namelist():
        f_bin = zf.read(filename)
        num_batches = int(len(f_bin)/chunk_bytes)
        print(num_batches)
        num_batches = 10
        assert float(len(f_bin)/chunk_bytes).is_integer()
        print("Number of batches:",num_batches)
        print("Total bytes ", len(f_bin))
        # Initialize batch arrays
        batch_imgs=np.ndarray((num_batches,max_pixels,max_pixels,max_colors),dtype=float32)
        batch_lbls=np.ndarray((num_batches,max_labels),dtype=float32)
        for i in range(num_batches):
            if i%1000 == 0:
                print("%d/%d=%.3f" %(i,num_batches,i/num_batches))
            offset = chunk_bytes*i
            img_tmp, lbl_tmp = _decode(f_bin,offset)
            batch_lbls[i] = np.asarray(lbl_tmp,dtype=float32).reshape(max_labels)
            for w in range(max_colors):
                reshaped_img = np.asarray(img_tmp[w]).reshape((max_pixels,max_pixels)) 
                for j in range(max_pixels):
                    for k in range(max_pixels):
                        batch_imgs[i][j][k][w]=reshaped_img[j][k]
        if (DEBUG) :
            for i in range(num_batches):
                print("EventID: ",i)
                print(batch_lbls[i])
                plt.figure(figsize=(15,10))
                for j in range(max_colors):
                    cd_idx = 230 + j + 1
                    plt.subplot(cd_idx)
                    plt.grid()
                    plt.imshow(batch_imgs[i][:,:,j])
                plt.show()    
        # Define TF record options
        compression = tf.python_io.TFRecordCompressionType.GZIP
        tf_io_opts = tf.python_io.TFRecordOptions(compression)
        # #Write data
        write_array_to_tfrecord(batch_imgs,batch_lbls,output_path,options=tf_io_opts)
        # except:
        #     print("c")
        #     zf.close()
