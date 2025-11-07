import socket
import threading

HOST = "127.0.0.1"
PORT = 3000

def run_client(client_id, message):
    """Send a message to the server and receive echo."""
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((HOST, PORT))
    print(f"[CLIENT {client_id}] Connected to server.")
    client.sendall(message.encode())
    data = client.recv(1024)
    print(f"[CLIENT {client_id}] Received: {data.decode()}")
    client.close()

def main():
    """Create multiple clients using threads."""
    messages = [
        "Hello from Client 1",
        "This is Client 2",
        "Client 3 checking in",
        "Greetings from Client 4",
        "Hi, this is Client 5"
    ]
    for i, msg in enumerate(messages, start=1):
        thread = threading.Thread(target=run_client, args=(i, msg))
        thread.start()

if __name__ == "__main__":
    main()
