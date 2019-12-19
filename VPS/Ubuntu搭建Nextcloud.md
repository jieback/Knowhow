# 安装snapd
```
sudo apt install snapd
```

# 通过snap安装nxtcloud
```
sudo snap install nextcloud
```
# 修改默认端口80 => 20080
 只是为了避免和其他的web程序冲突，非必需
```
sudo snap set nextcloud ports.http=20080
```
