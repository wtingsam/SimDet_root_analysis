import numpy as np
from numpy import *
from glob import glob
import tensorflow as tf
import sys
import os
import struct
import zipfile

LABEL_NAMES = ["p_t", "p_z",
               "entry_x", "entry_y", "entry_z"]
max_pixels = 128
max_colors = 2
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
    # Read header
    iev = struct.unpack('h',data[0+offset:2+offset])[0]
    nhits = struct.unpack('h',data[2+offset:4+offset])[0]
    print("offset: ", offset)
    # Read Labels
    for j in range(max_labels):
        from_j = int(4*(j+1)+offset)
        to_j = int(4+from_j)
        labels.append(struct.unpack('f',data[from_j:to_j])[0])
    if(DEBUG):
        print(labels)
        # Read images
    for j in range(max_colors):
        img_tmp = []
        for ip in range(max_pixels*max_pixels):
            from_ip = 24+4*ip+offset  + max_pixels*max_pixels*j
            to_ip   = 24+4*(ip+1)+offset + max_pixels*max_pixels*j
            img_pix = struct.unpack('f',data[from_ip:to_ip])[0]
            img_tmp.append(img_pix)
        images.append(img_tmp)
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
    chunk_bytes = 2 + 2 + 4*5 + max_pixels*max_pixels*4*max_colors
    for filename in zf.namelist():
        f_bin = zf.read(filename)
        num_batchs = int(len(f_bin)/chunk_bytes)
        print(num_batchs)
        num_batchs = 1
        print("Number of batches:",num_batchs)
        print("Total bytes ", len(f_bin))
        # Initialize batch arrays
        batch_imgs=np.ndarray((num_batchs,max_pixels,max_pixels,max_colors),dtype=float32)
        batch_lbls=np.ndarray((num_batchs,max_labels),dtype=float32)
        batch_lbls.fill(0)        
        load_batches = 0
        for i in range(num_batchs):
            if i%1000 == 0:
                print("%d/%d=%.3f" %(i,num_batchs,i/num_batchs))
            offset = chunk_bytes*i
            img_tmp, lbl = _decode(f_bin,offset)
            load_batches=load_batches+1
            for j in range(max_colors):
                a_img = np.asarray(img_tmp[j],dtype=float32).reshape(max_pixels,max_pixels)
                batch_imgs[i][:,:,j] = a_img
            batch_lbls[i] = np.asarray(lbl,dtype=float32).reshape(max_labels)
        print(batch_imgs)
        print(batch_lbls)
        for img in batch_imgs:
            print(img.shape)
            for i in range(max_pixels):
                for j in range(max_pixels):
                    if(img[i][j][1]!=0):
                        print("o",end="", flush=True)
                    else:
                        print("-",end="", flush=True)
                print("")
            
        # Define TF record options
        compression = tf.python_io.TFRecordCompressionType.GZIP
        tf_io_opts = tf.python_io.TFRecordOptions(compression)
        # #Write data
        write_array_to_tfrecord(batch_imgs,batch_lbls,output_path,options=tf_io_opts)
        # except:
        #     print("c")
        #     zf.close()
