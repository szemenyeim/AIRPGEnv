import socket
import threading,time
from gui import GUI
from characters import Monster, Hero, Character

port = 54000
ipAddress = '127.0.0.1'
playerName = 'Apa'
name = "AiRPG"
image= "map2.jpg"

CHARACTERS = {}
KEY_PRESSED = 0
my_xpos = my_ypos = my_id = 0
key = threading.Lock()


def draw_all(message, player):
    global my_id,my_ypos,my_xpos
    key.acquire()
    characters_from_msg = message.split("\n")
    for msg_it in characters_from_msg:
        params = msg_it.split(";")

        if len(params)== 2 and params[1] == "DEAD":
            CHARACTERS.pop(params[0])

        if len(params) != 7:
            continue

        try:
            id = params[0]
            x_pos = params[2]
            y_pos = params[3]
            level = params[4]
            currentHP = params[5]
            maximumHP= params[6]
        except:
            continue
        name = params[1]

        is_there = CHARACTERS.__contains__(id)

        if not is_there:
            if name == 'Monster':
                CHARACTERS[id] = Monster(x_pos, y_pos, level, maximumHP,currentHP)

            else:
                CHARACTERS[id] = Hero(x_pos, y_pos, level, maximumHP,currentHP)
                if name == playerName:
                    my_xpos = int(x_pos)
                    my_ypos = int(y_pos)
                    my_id = id

        elif is_there:
            CHARACTERS[id].position = (x_pos, y_pos)
            CHARACTERS[id].max_HP = maximumHP
            CHARACTERS[id].curr_HP = currentHP

            if my_id == id:
                my_xpos = int(x_pos)
                my_ypos = int(y_pos)
    key.release()


def invalidate(interface: GUI):
    x = interface.current_game
    interface.clear_window()
    global my_xpos,my_ypos
    for id in CHARACTERS:
        CHARACTERS[id].draw(interface)
    interface.show_window(my_xpos, my_ypos)

def client(ipAddress, port, player):
    global KEY_PRESSED
    # create an ipv4 socket object
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # connect the client
    client.connect(('127.0.0.1', 54000))
    client.setblocking(False)

    # send the name of the player to the server
    client.send(player.encode())
    received = 0
    while True:

        try:
            received = client.recv(4096)
        except:
            pass

        if received:
            message = received.decode()
            print(message)
            draw_all(message, player)
        received = None

        key.acquire()
        telegram = player + ":" + str(KEY_PRESSED)
        if KEY_PRESSED > 0:
            client.send(telegram.encode())
            KEY_PRESSED = 0
        key.release()
        time.sleep(0.01)


if __name__ == "__main__":
    client_thread = threading.Thread(target = client, args = (port, ipAddress, playerName))
    client_thread.start()

    gui = GUI(img4map=image, window_name=name)

    while True:

        key.acquire()
        KEY_PRESSED = gui.get_key_pressed()
        invalidate(gui)
        key.release()

        time.sleep(0.01)
