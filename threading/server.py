import socket
import threading

HOST = "127.0.0.1"   # localhost
PORT = 3000          # port number

def handle_client(client_socket, address):
    
    print(f"[CONNECTED] Client {address} connected.")
    while True:
        data = client_socket.recv(1024)
        if not data:
            break
        print(f"[RECEIVED from {address}] {data.decode()}")
        client_socket.sendall(data)  # echo message back to client
    print(f"[DISCONNECTED] Client {address} disconnected.")
    client_socket.close()

def main():
    """Start the server and listen for client connections."""
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, PORT))
    server.listen()
    print(f"[SERVER STARTED] Listening on {HOST}:{PORT}")

    try:
        while True:
            client_socket, addr = server.accept()
            thread = threading.Thread(target=handle_client, args=(client_socket, addr))
            thread.start()
            print(f"[ACTIVE CONNECTIONS] {threading.active_count() - 1}")
    except KeyboardInterrupt:
        print("\n[SERVER STOPPED]")
    finally:
        server.close()

if __name__ == "__main__":
    main()
