"""Credits: https://steelkiwi.com/blog/working-tcp-sockets/"""

import select
import socket
import global_vars as gv


class Server():
    def __init__(self):
        self.IP = '127.0.0.1'
        self.PORT = 54000
        self.inputs = []

        # build the server socket
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setblocking(0)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        try:
            self.server.bind((self.IP, self.PORT))
        except:
            self.server.connect((self.IP, self.PORT))

        self.server.listen()

        # add server socket object to the list of readable connections
        self.inputs.append(self.server)

    def broadcast(self):
        """broadcast chat messages to all connected clients"""
        gv.out_lock.acquire()
        for message in gv.mailbox_out:
            if gv.mailbox_out[message].sent == False:
                gv.mailbox_out[message].sent = True
                for s in self.inputs:
                    if s != self.server:
                        try:
                            s.send(gv.mailbox_out[message].text.encode())
                        except:
                            continue
        gv.out_lock.release()

    def msg_received(self, message,sock):
        message = message.split("\n\r")
        for msg in message:
            if msg != "":
                msg = msg.split(":")
                gv.mailbox_in [sock].append (msg)

    def remove_connection(self, sock):
        self.inputs.remove (sock)
        sock.close ()
        gv.server_lock.acquire ()
        gv.mailbox_in.pop (sock)
        gv.server_lock.release ()
        try:
            gv.con_lock.aquire()
            gv.cons.pop(sock)
            gv.con_lock.release()
        except:
            pass
    def run_server(self):
        outputs = []
        # gv.mailbox_in = {}

        while True:
            readable, _, _ = select.select (self.inputs, outputs, self.inputs)
            for sock in readable:

                # a new connection request recieved
                if sock is self.server:
                    connection, client_address = self.server.accept()
                    connection.setblocking(0)
                    self.inputs.append(connection)
                    gv.server_lock.acquire ()
                    gv.mailbox_in [connection] = []
                    gv.server_lock.release ()

                # a message from a client, not a new connection
                else:
                    try:
                        if gv.cons[sock] == None:
                            self.remove_connection(sock)
                            continue
                    except:
                        pass
                    # process data recieved from client
                    try:
                        data = sock.recv(4096)
                    except:
                        data = 0
                    if data:
                        message = data.decode()

                        # receiving data from the socket.
                        self.msg_received(message,sock)
                    else:
                        self.remove_connection(sock)
            self.broadcast()

        self.server.close()


if __name__ == "__main__":
    print("server test")
    s = Server()
    s.run_server()
