import socket
import threading
import time

def div_remainder(val, div):
    
    return int(val // div), int(val % div)
    
def time_format(milliseconds):
    
    s, ms = div_remainder(milliseconds, 1000)
    m, s = div_remainder(s, 60)
    h, m = div_remainder(m, 60)
    
    if h == 0:
        return '{0:02d}:{1:02d}.{2:03d}'.format(m, s, ms)
    else:
        return '{0:02d}:{1:02d}:{2:02d}.{3:03d}'.format(h, m, s, ms)

def subtitles_thread():
    print("subtitles_thread started")
    
    conn = socket.create_connection(('localhost', 9090))
    
    header = "WEBVTT\n\n"
    conn.send(bytes(header, "utf-8"))
    print(header, end='')
    
    template = "{0} --> {1}\n{2}\n\n"
    
    counter_ms = 0
    delta_ms = 1000
    while True:
        
        curr_time = time_format(counter_ms)
        next_time = time_format(counter_ms + delta_ms)
        msg = 'Current time: {0}'.format(curr_time)
        
        s = template.format(curr_time, next_time, msg)
        conn.send(bytes(s, 'utf-8'))
        print(s, end='')
        
        counter_ms += delta_ms
        time.sleep(delta_ms / 1000.0)
    
    conn.close()
    
    print("subtitles_thread finished")


if __name__ == '__main__':
    
    t1 = threading.Thread(target=subtitles_thread)

    print('STARTING')
    t1.start()

    print('JOINING')
    t1.join()
