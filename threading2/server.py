import socket
import threading


HOST = "127.0.0.1"
PORT=3000



def handle_client(client_socket, addr):
    print(f"[CONNECTED] Client {addr} connected.")
    while True:
        data = client_socket.recv(1024)
        if not data:
            break
        print(f"[RECEIVED from {addr}] {data.decode()}")
        client_socket.sendall(data)  # echo back
    print(f"[DISCONNECTED] Client {addr} disconnected.")
    client_socket.close()





def main():
    server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    server.bind((HOST,PORT))
    server.listen()
    print(f"[SERVER STARTED] Listening on {HOST}:{PORT}")


    try:
        while True:
            client_socket , add = server.accept()
            thread =threading.Thread(target=handle_client, args=(client_socket,add))
            thread.start()
    except KeyboardInterrupt:
        print("error")
    finally:
        server.close()



if __name__ =="__main__":
    main()