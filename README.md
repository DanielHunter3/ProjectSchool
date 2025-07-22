Руководство по сборке и запуске файлов в проекте
-------------------------------------------------
1. Скомпилируем файлы первых двух тестов в папке ***build***
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
Файлы скомпилировались
-------------------------------------------------
2. Запустим 5 тестов с Pool Allocator Singlethread
```bash
for i in {1..5}; do     ./single_pool   ; done
```
Проверка
-------------------------------------------------
3. Запустим 5 тестов с Pool Allocator Multithread
```bash
for i in {1..5}; do     ./multi_pool   ; done
```
Провека
-------------------------------------------------
4. Запустим 5 тестов с Vector Singlethread
```bash
for i in {1..5}; do     ./single_vec    ; done
```
Проверка
-------------------------------------------------
5. Запустим 5 тестов с Vector Multithread
```bash
for i in {1..5}; do     ./multi_vec    ; done
```
Проверка
-------------------------------------------------
6. Перейдем в корень проекта и в папку ***alloc***:
```bash
cd ..
cd alloc
```
Далее...
-------------------------------------------------
7. Скомпилируем файлы третьего теста в их личной папке
```bash
g++ -O3 -pthread -DUSE_JEMALLOC alloc_multithread.cpp -ljemalloc -o je_mt
g++ -O3 -DUSE_JEMALLOC alloc_singlethread.cpp -ljemalloc -o je_st
g++ -O3 -pthread alloc_multithread.cpp -o ma_mt
g++ -O3 alloc_singlethread.cpp -o ma_st
```
Файлы скомпилировались
-------------------------------------------------
8. Запустим 5 тестов с Alloc Singlethread
```bash
for i in {1..5}; do  ./ma_st;  ./je_st  ; done
```
Проверка
-------------------------------------------------
9. Запустим 5 тестов с Alloc Singlethread
```bash
for i in {1..5}; do  ./ma_mt;  ./je_mt  ; done
```
Проверка
-------------------------------------------------
Конец