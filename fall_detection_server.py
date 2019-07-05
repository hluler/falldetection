import socket
import signal
import errno
import time
import os
import sys
from statistics import median
from statistics import stdev
from scipy.stats import kurtosis,skew
import math
#import tensorflow as tf
#from tensorflow import keras
import numpy as np
import pandas as pd
import csv
from sklearn import svm
from random import randint
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_auc_score
from sklearn.metrics import roc_curve
from sklearn.metrics import precision_recall_curve
from sklearn.metrics import f1_score
from sklearn.metrics import auc
from sklearn import datasets
from joblib import dump, load


sum_df = pd.DataFrame()
start_index = 0
DETECT_STEP = 200
FALL_SIZE = 1200

strHost = "109.123.123.130"
HOST = strHost #socket.inet_pton(socket.AF_INET,strHost)
PORT = 8080

httpheader = '''''\ 
HTTP/1.1 200 OK 
Context-Type: text/html 
Server: Python-slp version 1.0 
Context-Length: '''


def remove_old_data():
    print("remove old data")
    global sum_df
    sum_df = sum_df.iloc[start_index:,]
    #sum_df.drop([0,start_index], axis='rows')

def process_data():
    X = []
    Y = []
    Z = []
    MAG = []
    ymag = []
    for i in range(start_index,FALL_SIZE+start_index):
        #print("current index   ", i)
        X.append(sum_df.iloc[i, 0])
        Y.append(sum_df.iloc[i, 1])
        Z.append(sum_df.iloc[i, 2])
        MAG.append(sum_df.iloc[i, 7])
        ymag.append(float(Y[i-start_index])/float(math.sqrt(MAG[i-start_index])))
    TA = [math.asin(ymag[k]) for k in range(0,FALL_SIZE)]
    avgX = sum(X)/len(X)
    avgY = sum(Y)/len(Y)
    avgZ = sum(Z)/len(Z)
    medianX = median(X)
    medianY = median(Y)
    medianZ = median(Z)
    try:
        stdX = stdev(X)
    except:
        stdX = 0
    try:
        stdY = stdev(Y)
    except:
        stdY = 0
    try:
        stdZ = stdev(Z)
    except:
        stdZ = 0
    #stdX = stdev(X)
    #stdY = stdev(Y)
    #stdZ = stdev(Z)
    skewX = skew(X)
    skewY = skew(Y)
    skewZ = skew(Z)
    kurtosisX = kurtosis(X)
    kurtosisY = kurtosis(Y)
    kurtosisZ = kurtosis(Z)
    minX = min(X)
    minY = min(Y)
    minZ = min(Z)
    maxX = max(X)
    maxY = max(Y)
    maxZ  = max(Z)
    slope = math.sqrt((maxX - minX)**2 + (maxY - minY)**2 + (maxZ - minZ)**2)
    meanTA = sum(TA)/len(TA)
    stdTA = stdev(TA)
    skewTA = skew(TA)
    kurtosisTA = kurtosis(TA)

    absX = sum([abs(X[k] - avgX) for k in range(0,FALL_SIZE) ]) / len(X)
    absY = sum([abs(Y[k] - avgY) for k in range(0,FALL_SIZE) ]) / len(Y)
    absZ = sum([abs(Z[k] - avgZ) for k in range(0,FALL_SIZE) ]) / len(Z)
    abs_meanX = sum([abs(X[k]) for k in range(0,FALL_SIZE)])/len(X)
    abs_meanY = sum([abs(Y[k]) for k in range(0,FALL_SIZE)])/len(Y)
    abs_meanZ = sum([abs(Z[k]) for k in range(0,FALL_SIZE)])/len(Z)
    abs_medianX = median([abs(X[k]) for k in range(0,FALL_SIZE)])
    abs_medianY = median([abs(Y[k]) for k in range(0,FALL_SIZE)])
    abs_medianZ = median([abs(Z[k]) for k in range(0,FALL_SIZE)])
    try:
        abs_stdX = stdev([abs(X[k]) for k in range(0,FALL_SIZE)])
    except:
        abs_stdX = 0

    try:
        abs_stdY = stdev([abs(Y[k]) for k in range(0,FALL_SIZE)])
    except:
        abs_stdY = 0

    try:
        abs_stdZ = stdev([abs(Z[k]) for k in range(0,FALL_SIZE)])
    except:
        abs_stdZ = 0
        
    abs_skewX = skew([abs(X[k]) for k in range(0,FALL_SIZE)])
    abs_skewY = skew([abs(Y[k]) for k in range(0,FALL_SIZE)])
    abs_skewZ = skew([abs(Z[k]) for k in range(0,FALL_SIZE)])
    abs_kurtosisX = kurtosis([abs(X[k]) for k in range(0,FALL_SIZE)])
    abs_kurtosisY = kurtosis([abs(Y[k]) for k in range(0,FALL_SIZE)])
    abs_kurtosisZ = kurtosis([abs(Z[k]) for k in range(0,FALL_SIZE)])
    abs_minX = min([abs(X[k]) for k in range(0,FALL_SIZE)])
    abs_minY = min([abs(Y[k]) for k in range(0,FALL_SIZE)])
    abs_minZ = min([abs(Z[k]) for k in range(0,FALL_SIZE)])
    abs_maxX = max([abs(X[k]) for k in range(0,FALL_SIZE)])
    abs_maxY = max([abs(Y[k]) for k in range(0,FALL_SIZE)])
    abs_maxZ  = max([abs(Z[k]) for k in range(0,FALL_SIZE)])
    abs_slope = math.sqrt((abs_maxX - abs_minX)**2 + (abs_maxY - abs_minY)**2 + (abs_maxZ - abs_minZ)**2)
    meanMag = sum(MAG)/len(MAG)
    try:
        stdMag = stdev(MAG)
    except:
        stdMag = 0
    minMag = min(MAG)
    maxMag = max(MAG)
    DiffMinMaxMag = maxMag - minMag
    ZCR_Mag = 0
    AvgResAcc = (1/len(MAG))*sum(MAG)

    test = [avgX,avgY,avgZ,medianX,medianY,medianZ,stdX,stdY,stdZ,skewX,skewY,skewZ,kurtosisX,kurtosisY,kurtosisZ,minX,minY,minZ,maxX,maxY,maxZ,slope,meanTA,stdTA,skewTA,kurtosisTA,absX,absY,absZ,abs_meanX,abs_meanY,abs_meanZ,abs_medianX,abs_medianY,abs_medianZ,abs_stdX,abs_stdY,abs_stdZ,abs_skewX,abs_skewY,abs_skewZ,abs_kurtosisX,abs_kurtosisY,abs_kurtosisZ,abs_minX,abs_minY,abs_minZ,abs_maxX,abs_maxY,abs_maxZ,abs_slope,meanMag,stdMag,minMag,maxMag,DiffMinMaxMag,ZCR_Mag,AvgResAcc]
    #final.append(test)

    return test


def feature(FILE_PATH):

    df_list = []
    
    count = 0
    final = []
    for file in os.listdir(FILE_PATH):

        df = pd.read_csv(os.path.join(FILE_PATH,file))
        print(file)
        df["acc_x_temp"]= df["acc_x"].astype('float64')
        df["acc_y_temp"]= df["acc_y"].astype('float64')
        df["acc_z_temp"]= df["acc_z"].astype('float64')

        #
        df["acc_x"]= df["acc_x_temp"].astype('float64')
        df["acc_y"]= df["acc_z_temp"].astype('float64')
        df["acc_z"]= df["acc_y_temp"].astype('float64')

        df['mag'] = df['acc_x']*df['acc_x'] + df['acc_y']*df['acc_y'] + df['acc_z']*df['acc_z']
        #mag = math.sqrt(df['acc_x']*df['acc_x'] + df['acc_y']*df['acc_y'] + df['acc_z']*df['acc_z'])

        global sum_df
        sum_df = pd.concat([sum_df, df])

        print(df.head())
        #if sum_df.shape[0] == 0:
        #    sum_df = df
        #else:
        #    print("append df to global df")
        #    sum_df = sum_df.append(df, ignore_index = True)

        sum_df.reset_index(drop=True)
        global start_index
        df_count = sum_df.shape[0]
        print("total count", df_count, "current index is ",start_index)
        while(df_count>=(FALL_SIZE+start_index)):
            test = process_data()
            final.append(test)
            start_index = start_index + DETECT_STEP

        if (start_index > 5*FALL_SIZE):
            remove_old_data()
            start_index = 0
        
    return final

def HttpResponse(name, header):
    response = "%s %d\n\n%s\n\n" % (header, len(name), name)
    return response

def sigIntHander(signo,frame):
    print('get signo# ',signo)
    global runflag
    runflag = False
    global lisfd
    lisfd.shutdown(socket.SHUT_RD)

if __name__=='__main__':

    lisfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    #lisfd.settimeout(CHECK_TIMEOUT)  
    lisfd.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)         
    lisfd.bind(("", 8080))
    lisfd.listen(2)
    signal.signal(signal.SIGINT, sigIntHander)

    while 1:
        try:
            print("====fall dector data receive====")
            confd, addr = lisfd.accept()
        except socket.error as e:
            print("server error")
            if e.errno == errno.EINTR:
                print('get a except EINTR')


        print("connect by ", addr)
        tm=time.time()
        data = []
        lendata = []
        buf = []
        hasdatalen = True
        rlen = 0
        datalen = 0x7fffffff
        while rlen < datalen:
            buf = confd.recv(4096)
            #print(buf)
            rlen += len(buf)
            #print("Target:",datalen,", already:",rlen,", new ",len(buf),"Bytes")
            if (len(buf)==0): break
            #Req = str(buf,  encoding="utf-8")
            #buf.encode(encoding='UTF-8',errors='strict')
            Req = str(buf,"utf-8")
            #buf.decode()
            #Req = buf
            #print Req
            #Get Content-Length length
            if hasdatalen:
                        x1 = Req.index("Content-Length")
                        tstr = Req[x1 + 16: x1 + 30]
                        x2 = tstr.index('\r\n')
                        datalen = int(tstr[0:x2])
                        print(datalen)
                        hasdatalen = False

            #print(len(Req))
            data.append(Req)
        #print(data)
        webdata = ''.join(data)
        #print(webdata)

        print("Begin decode")
        wlen = len(webdata)

        begin = webdata.index("octet-stream:")
        end = webdata.index("endname");
        file_name = webdata[begin + len("octet-stream:"):end]
        base=os.path.splitext(file_name);
        print("file_name:",file_name,",base:",base[0],"...")
        
        begin = webdata.index('\r\n\r\n')
        #print "webdata:",webdata,"...."
        #print "begin:", begin, "..."
        sStr1 = webdata[begin + 4:wlen]
        #print "sStr1:",sStr1,"..."
        print("Decode Done")


        print("Begin Train")
        file_path = "/home/helong/share/ML/fall_detection_real_data/"+base[0]+".csv";
        file = open(file_path, 'w+')
        file.writelines("acc_x,acc_y,acc_z,timestamp\n")
        file.writelines(sStr1)
        file.close()
        print("*********Write[",file_path,"]cost:",time.time()-tm)
        time.sleep(0.2)
        exception_flag = 0
        l_data = feature("/home/helong/share/ML/fall_detection_real_data/")
        try: 
            
            clf_load = load('/home/helong/share/ML/fall_detect_svm.joblib')
            y_predict = clf_load.predict(l_data)
        except:
            exception_flag = 1

        #np.round(np.clip(y_predict, 0, 1))
        print("*********Train cost:",time.time()-tm)
        for i in range(len(y_predict)):
            print("Predict result :", y_predict[i])
            if(exception_flag == 1 or y_predict[i] == 0):
                Res = bytes("normal state", encoding="utf-8")
            else:
                Res = bytes("attention, fall dectected", encoding="utf-8")
                confd.send(Res)
                break
            print(Res)
        confd.close()



