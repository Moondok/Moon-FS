# <center>  Moon-FS: File System of Unix-Like OS
## Brief intro
This is a secondary file system of a unix-like operating system, a file "Disk.img" with 64MB memory space is used to simulate a disk device.
## Usage

```
git clone https://github.com/Moondok/Moon-FS.git
cd Moon-FS
mkdir build && cd build && make
./Moon_FS
```

## Test
you can test our file system via following test cases.
```
mkdir home/texts
mkdir home/reports
mkdir home/photos
touch home/photos/disparity.png
```

## Checklist
- [x] mkdir
- [x] ls
- [x] touch
- [ ] stat
- [ ] mv
- [ ] cd



