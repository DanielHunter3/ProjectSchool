Руководство по сборке и запуске файлов в проекте
-------------------------------------------------
Заранее скачать:
```bash
sudo apt install libgoogle-perftools-dev
sudo apt install libmimalloc-dev
sudo apt install libjemalloc-dev
sudo apt install libboost-dev libboost-system-dev
```
А также могут понадобиться:
1. C++ compiler
2. make
3. cmake
Но я вряд ли найду Linux, в котором по умолчанию не будет этих инструментов,
поэтому не буду показывать инструкцию по их скачиванию
-------------------------------------------------
Заходите в папки ***boost*** и ***alloc***, запускайте в них build.sh через:
```bash
./build.sh
```
Но могут появиться проблемы с правами доступа. В этом случае пишем перед запуском:
```bash
chmod +x build.sh
```