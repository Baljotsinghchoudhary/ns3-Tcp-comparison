import pandas as pd 
import numpy as np
import matplotlib.pyplot as plt
tcp_variants=["TcpNewReno", "TcpHybla", "TcpHighSpeed","TcpHtcp","TcpVegas", "TcpScalable","TcpVeno" ,"TcpBic", "TcpYeah", "TcpIllinois", "TcpWestwood", "TcpWestwoodPlus", "TcpLedbat"]

segment = 360 




def get_data(type,protocol):
    data=pd.read_csv(f"scratch/acn-tcp-variant/{protocol}/{type}.data",sep="\s+",names=[0,1],engine='python')
   
    return data.T.to_numpy()
  

def wrappper(cwnd,statedata):
   xaxis=[]
   yaxis=[]
   counter=0
   state=0;
   for x in cwnd[0]:
        if counter!=len(statedata[0]) and statedata[0][counter]>=x:
           state=statedata[1][counter]
           counter+=1
        
        xaxis.append(x)
        yaxis.append(state)

   return statedata


def plotCwndSsthres(protocol):
    plt.figure(1)
    cwnddata=get_data("cwnd",protocol)
    ssthdata=get_data("ssth",protocol)
    statedata=get_data("cong-state",protocol)
    plt.plot(cwnddata[0], cwnddata[1] / segment,color='darkblue', label='cwnd',linewidth=2)
    plt.plot(ssthdata[0], ssthdata[1] / segment,color='red', linestyle='dotted', label='ssth',linewidth=2)
    plt.title(protocol)
    plt.ylim(0, cwnddata[1] .max()/segment +200)
    plt.ylabel('segment(1seg=360bytes)')
    plt.xlabel('Time(sec)')
    
    
    fill_data=wrappper(cwnddata,statedata)
    plt.axvspan(0,15,facecolor='lightblue', alpha=0.1)
    for i in range(0,len(fill_data[0])-1):
        if fill_data[1][i] == 1: #disorder
            plt.axvspan(fill_data[0][i],fill_data[0][i+1]+0.1,facecolor='green', alpha=0.5)
        elif fill_data[1][i]== 3: #recovery
           plt.axvspan(fill_data[0][i],fill_data[0][i+1]+0.1,facecolor='yellow', alpha=0.5)
        elif fill_data[1][i]== 4: #Loss
            plt.axvspan(fill_data[0][i],fill_data[0][i+1]+0.1,facecolor='red', alpha=0.5)
    plt.plot([],[],color='lightblue',label='open')
    plt.plot([],[],color='yellow',label='recovery')
    plt.plot([],[],color='green',label='disorder')
    plt.plot([],[],color='red',label='loss')
    plt.legend()
    plt.savefig(f'scratch/acn-tcp-variant/{protocol}/plot.png')
    plt.close()
    
def plotack(protocol):
    plt.figure(500)
    data=get_data("ack",protocol)
    plt.plot(data[0],data[1] / segment,label=protocol)






if __name__ == '__main__':
  
    for protocol in tcp_variants:
        plotCwndSsthres(protocol)
        plotack(protocol)
    plt.figure(500)
    plt.title("ACKNOWLEDGEMENT")
    plt.xlabel("TIME(ns)")
    plt.ylabel("SEGMENT(360bytes)")
    plt.legend()
    plt.savefig(f'scratch/acn-tcp-variant/ack-plot.png')
    plt.close()

  




