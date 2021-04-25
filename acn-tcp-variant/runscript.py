    #running tcp variants one by one
import os

tcp_variants=["TcpNewReno", "TcpHybla", "TcpHighSpeed","TcpHtcp","TcpVegas", "TcpScalable","TcpVeno" ,"TcpBic", "TcpYeah", "TcpIllinois", "TcpWestwood", "TcpWestwoodPlus", "TcpLedbat"]
for protocol in tcp_variants:
    os.system(f'./waf --run "scratch/acn-tcp-variant/acn-tcp-variant --transport_prot={protocol} --error_p=0.00" ')
    
