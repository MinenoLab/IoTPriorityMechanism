import binascii
if __name__ == "__main__":
    f=open("DummySensorDatas.txt",'w')
    strdata=""
    bytesize=80
    for i in range(bytesize):
    	strdata=strdata+"S"
    f.write(strdata)
