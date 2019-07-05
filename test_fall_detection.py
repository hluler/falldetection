from keras.models import Sequential
from keras.layers import LSTM
from keras.layers import Dense, Dropout
from numpy import genfromtxt
import numpy
import codecs
import csv
from sklearn.metrics import confusion_matrix,roc_auc_score,roc_curve,auc,accuracy_score
from sklearn.metrics import precision_score,recall_score,classification_report
import matplotlib.pyplot as plt
from keras.utils import np_utils
import numpy as np
from keras import optimizers

# create a sequence classification instance
def get_sequence(n_timesteps,time):
    x = [data[index] for index in range((time*10)+1 ,(time*10)+10+1)]
    x = numpy.delete(x, (data.shape[1]-1), axis=1)
    x= numpy.array(x)
    y = [Y[index] for index in range((time*10) ,(time*10)+10)]
    y=numpy.array(y)
    # reshape input and output data to be suitable for LSTMs
    X = x.reshape(1, n_timesteps, (data.shape[1]-1))
    y = y.reshape(1, n_timesteps, y.shape[1])
    return X, y



data = genfromtxt('/home/helong/Downloads/Fall-detection-master/Experiment1.csv', delimiter=',')
m =[data[i][-1] for i in range(1,data.shape[0])]
Y = np_utils.to_categorical(m)

print(data.shape[0])
print(data.shape[1])

# define problem properties
n_timesteps = 10
# define LSTM
model = Sequential()
model.add(LSTM(100,input_shape = (None, (data.shape[1]-1)),return_sequences=True))
#model.add(Dropout(0.25))
#model.add(Dense(1, activation='softmax'))
model.add(Dense(1, activation='sigmoid'))
opt = optimizers.SGD(lr=0.0001)
model.compile(loss='mean_squared_error', optimizer = "adam" , metrics=['acc'])

print(model.summary())


#train_len = 7600  #int(len(data)*0.7)
#valid_len = 7700 #int(len(data)*0.8)
#test_len = 7000  #len(data)-1
train_len = int(len(data)*0.7)
valid_len = int(len(data)*0.8)
test_len = len(data)-1

print("train_len is ",train_len)
print("valid_len is ",valid_len)
print("test_len is ",test_len)

x1 = [data[index] for index in range(4600, valid_len)]


#print(x1.head())

x1 = numpy.delete(x1, (data.shape[1]-1), axis=1)
x1 = numpy.array(x1)
y1 = [data[index][-1] for index in range(4600, valid_len)]
y1 = numpy.array(y1)
# reshape input and output data to be suitable for LSTMs
X1 = x1.reshape(1, valid_len-4600, data.shape[1]-1)
y1 = y1.reshape(1, valid_len-4600, 1)

x = [data[index] for index in range(1, train_len)]
x = numpy.delete(x, (data.shape[1]-1), axis=1)
x = numpy.array(x)
y = [data[index][-1] for index in range(1, train_len)]
y = numpy.array(y)
# reshape input and output data to be suitable for LSTMs
X = x.reshape(1, train_len-1, data.shape[1]-1)
y = y.reshape(1, train_len-1, 1)

x2 = [data[index] for index in range(6000, test_len)]
x2 = numpy.delete(x2, (data.shape[1]-1), axis=1)
x2 = numpy.array(x2)
y2 = [data[index][-1] for index in range(6000, test_len)]
y2 = numpy.array(y2)
# reshape input and output data to be suitable for LSTMs
X2 = x2.reshape(1, test_len-6000, data.shape[1]-1)
y2 = y2.reshape(1, test_len-6000, 1)

model.fit(X, y,batch_size=10, epochs = 1,verbose=1)

scores = model.evaluate(X1,y1,verbose=0)
print(scores[1]*100)


yhat = model.predict(X1,verbose=0)[0]


# train LSTM
for epoch in range(0,20):
	# generate new random sequence
	X,y = get_sequence(n_timesteps,epoch)
	# fit model for one epoch on this sequence
	model.fit(X, y,batch_size=1, verbose=0)
    
for epoch in range(30,50):
	# generate new random sequence
	X,y = get_sequence(n_timesteps,epoch)
	# fit model for one epoch on this sequence
	model.fit(X, y,batch_size=1, verbose=0)
    
for epoch in range(60,80):
	# generate new random sequence
	X,y = get_sequence(n_timesteps,epoch)
	# fit model for one epoch on this sequence
	model.fit(X, y,batch_size=1, verbose=0)    
    
for epoch in range(90,110):
	# generate new random sequence
	X,y = get_sequence(n_timesteps,epoch)
	# fit model for one epoch on this sequence
	model.fit(X, y,batch_size=1, verbose=0)

for epoch in range(120,140):
	# generate new random sequence
	X,y = get_sequence(n_timesteps,epoch)
	# fit model for one epoch on this sequence
	model.fit(X, y,batch_size=1, verbose=0)

countp=0
countn=0

ypredicted =  []
yactual = []



for d in range(20,30):
        X,y = get_sequence(n_timesteps,d)
        #yactual[] = [y[i] for i in range(len(y))]
        yhat = model.predict(X,verbose=0)[0]
        for index1 in range(10):
            i = np.where(yhat[index1] == yhat[index1].max())
            hin = i[0]
            for index2 in range(2):
                if(index2==hin):
                    yhat[index1][index2]=1
                else:
                    yhat[index1][index2]=0
        j= yhat
        k= y[0]
        for index1 in range(10):
            ypredicted.append(j[index1])
            yactual.append(k[index1])
         
            
for d in range(50,60):
        X,y = get_sequence(n_timesteps,d)
        #yactual[] = [y[i] for i in range(len(y))]
        yhat = model.predict(X,verbose=0)[0]
        for index1 in range(10):
            i = np.where(yhat[index1] == yhat[index1].max())
            hin = i[0]
            for index2 in range(2):
                if(index2==hin):
                    yhat[index1][index2]=1
                else:
                    yhat[index1][index2]=0
        j= yhat
        k= y[0]
        for index1 in range(10):
            ypredicted.append(j[index1])
            yactual.append(k[index1])
            
for d in range(80,90):
        X,y = get_sequence(n_timesteps,d)
        #yactual[] = [y[i] for i in range(len(y))]
        yhat = model.predict(X,verbose=0)[0]
        for index1 in range(10):
            i = np.where(yhat[index1] == yhat[index1].max())
            hin = i[0]
            for index2 in range(2):
                if(index2==hin):
                    yhat[index1][index2]=1
                else:
                    yhat[index1][index2]=0
        j= yhat
        k= y[0]
        for index1 in range(10):
            ypredicted.append(j[index1])
            yactual.append(k[index1])
        
for d in range(110,120):
        X,y = get_sequence(n_timesteps,d)
        #yactual[] = [y[i] for i in range(len(y))]
        yhat = model.predict(X,verbose=0)[0]
        for index1 in range(10):
            i = np.where(yhat[index1] == yhat[index1].max())
            hin = i[0]
            for index2 in range(2):
                if(index2==hin):
                    yhat[index1][index2]=1
                else:
                    yhat[index1][index2]=0
        j= yhat
        k= y[0]
        for index1 in range(10):
            ypredicted.append(j[index1])
            yactual.append(k[index1])

for d in range(140,150):
        X,y = get_sequence(n_timesteps,d)
        #yactual[] = [y[i] for i in range(len(y))]
        yhat = model.predict(X,verbose=0)[0]
        for index1 in range(10):
            i = np.where(yhat[index1] == yhat[index1].max())
            hin = i[0]
            for index2 in range(2):
                if(index2==hin):
                    yhat[index1][index2]=1
                else:
                    yhat[index1][index2]=0
        j= yhat
        k= y[0]
        for index1 in range(10):
            ypredicted.append(j[index1])
            yactual.append(k[index1])
            

            
ya = []
yp = []


for index1 in range(len(ypredicted)):
            if (ypredicted[index1][0]==1 and ypredicted[index1][1]==0 ):
                yp.append(0)
            if (ypredicted[index1][0]==0 and ypredicted[index1][1]==1 ):
                yp.append(1)


for index1 in range(len(yactual)):
            if (yactual[index1][0]==1 and yactual[index1][1]==0 ):
                ya.append(0)
            if (yactual[index1][0]==0 and yactual[index1][1]==1 ):
                ya.append(1)


for i in range(len(ya)):
    if(ya[i] == yp[i]):
                countp = countp+1
    else:
        if(i!=len(ya)-1):
            if((ya[i+1]!=0 and yp[i]!=0) or (ya[i-1]!=0 and yp[i]!=0)):
                countp=countp+1
            else:
                countn = countn+1
        else:
            countn = countn+1


yaa=[]
ypp=[]
for i in range(len(ya)):
    if(ya[i]==yp[i]):
        yaa.append(ya[i])
        ypp.append(yp[i])
    else:
        if(ya[i]!=yp[i]):
            if((ya[i]==1 and yp[i+1]==1) or (ya[i-1]==1 and yp[i]==1) or (ya[i]==1 and yp[i-1]==1) or (ya[i+1]==1 and yp[i]==1)):
                yaa.append(1)
                ypp.append(1)
            else:
                yaa.append(ya[i])
                ypp.append(yp[i])

conf_arr = confusion_matrix(yaa, ypp)
conf_arr_1 = confusion_matrix(ya, yp)

Precision = precision_score(yaa, ypp,average='micro') 
Recall = recall_score(yaa, ypp,average='micro')
classification_report(yaa, ypp)
print(classification_report(ya, yp))
print(accuracy_score(ya,yp))
