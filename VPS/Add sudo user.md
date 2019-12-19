```
root@vultr:~# adduser newuser
New password:
Retype new password:
```

```
root@vultr:~# cat /etc/group | grep sudo
sudo:x:27:ubuntu
```

'''
root@vultr:~# usermod -G sudo -a newuser
```

```
root@vultr:~# cat /etc/group | grep sudo
sudo:x:27:ubuntu,newuser
```
