import socket,time,threading


def tcpLink(sock, addr):
    print('Accept new connection from %s:%s...' % addr)
    sock.send('Welcome!'.encode())  # 当有新的连接到来时，向其回复Welcome!
    while True:
        data = sock.recv(1024).decode()      # 单次最多接收 1K 
        time.sleep(1)
        if data == 'exit':
            break
        print('RECV: %s' % data)
    sock.close()
    print('Connection from %s:%s closed.' % addr)


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# 监听端口:
s.bind(('192.168.137.1', 8080))
s.listen(5)
print('Waiting for connection...')
while True:
    # 接受一个新连接:
    sock, addr = s.accept()
    # 创建新线程来处理TCP连接:
    t = threading.Thread(target=tcpLink, args=(sock, addr))
    t.start()
