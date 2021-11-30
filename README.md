# RAWRP

It just sends an ARP request.

## Connection schema:
  - the method for sending data is specified on net.c@send_packet, it goes like this
  
  1. Opens a socket with socket():
 
    -Domain: PF_PACKET
    -Type:   SOCK_RAW
    -Proto:  ETH_P_ALL
    
  2. Creates an address struct (sockaddr_ll) to specify our interface:
  
    -sll_family:    AF_PACKET
    -sll_ifindex:   (the index of our network interface calculated at net.c@get_if_info)
    -sll_protocol:  ETH_P_ALL
    
  3. Binds the socket to the network card with bind():
  
    -Socket:  sockfd
    -Address: sll
    -Addrlen: sizeof(sll)
    
  3. Writes data to the socket with write():
  
    -Descriptor: sockfd
    -Data:       payload to send
    -Length:     len of payload
   
###TLDR:

  s = socket()
  sockaddr_ll sll
  sll. [] = stuff
  bind(s, sll, sizeof(sll))
  write(s, payload, len)
  close(s)
   
## ARP packets:

  ARP packets are handcrafted in arp.c
## ETH frames:

  ETH frames are also handcrafted in ether.c
  CRC calculations were boring so i stole the code from somewhere i cannot recall crc.c
  
##Usage:
  1. Edit the call to rawrp in main.c with your interface name and the ip to request MAC for.
  2. save changes and compile with gcc *.c
  3. execute with ./a.out

