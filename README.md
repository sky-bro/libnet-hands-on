
```shell
-i 219.217.228.102 ---- dst_ip
-p 80 ---- dst_port
-d wlp3s0 ---- device
-l 10 ---- loopCount
-s 0.5 ---- send-syn delay
```

* 运行方式 ` sudo ./build/forgetcp -d eth0 `