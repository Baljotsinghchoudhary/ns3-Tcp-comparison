# Requirements:
* Linux system
* Ns-3 network simulation.
* Python 3
* Numpy, Pandas, Matplotlib for graph plotting

# Installation Steps:

1. Go to [link](https://github.com/Baljotsinghchoudhary/ns3-Tcp-comparison) and place the folder in scratch directory of ns3 
2. Navigate to ns3 folder /ns-3.30 and open terminal
3. Run <strong> Python3 scratch/acn-tcp-variant/runscript.py </strong>
4. After this you can see various folder with name < tcp variant > in acn-tcp-variant folder
5. 	Content of each <tcp-variant> folder
    * ack.data : acknowledged sequence no with time
    * ascii.tr: Ascii tracing file
    * .pcap: Packet capture file
    * anim.xml: Animator file of Visualizing 
    * Flow.xml: Flow Monitor File
    * Cwnd.data: Congestion window data file
    * Ssth.data: slow-start threshold data file
    * Rtt.data: round-trip time data file
    * Cong-state.data: Congestion state data file
6. Navigate to ns-3.30 folder and Run <strong> python3 scratch/acn-tcp-variant/plotscript.py</strong>
7. Now you can see plot.png in each <tcp-variant> folder and ack.png in ack.png in acn-tcp-variant folder these file will be shown later
8. To parse flow monitor file either open in netanim and change stats to flow monitor file or copy src/flow-monitor/examples/flowmon-parse-result.py into ns3 folder and Run command <strong>python2 flowmon-parse-result.py scratch/acn-tcp-variant/<name>/flow.xml</strong>
9. To visualize Use netanim and open anim.xml file in NetAnim
10. To Analyse  Ascii trace download tracemetrics and open file in it
11. To analyse Pcap file open file in wireshark

# PLOTS
![IMG](/acn-tcp-variant/TcpNewReno/plot.png)
![IMG](/acn-tcp-variant/TcpScalable/plot.png)
![IMG](/acn-tcp-variant/TcpWestwood/plot.png)
![IMG](/acn-tcp-variant/ack-plot.png)

 


