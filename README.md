# Tape Sorter

## Функционал

 - CLI
 - Поддержка Linux/Windows
 - конфигурация в формате TOML

## Использование

### CLI

Программа использует CLI для передачи параметров: входного файла ленты, выходного файла ленты. Также необязательным параметром является `--config`, позволяющий указать путь до файла конфигурации.

```
Usage: tape_sorter [--help] [--version] [--config VAR] input output

Positional arguments:
  input          path to input tape file 
  output         path to output tape file 

Optional arguments:
  -h, --help     shows help message and exits 
  -v, --version  prints version information and exits 
  -c, --config   path to configuration file (default: "config.toml") 
```

### Файл конфигурации

Задание искусственной задержки работы ленты, а также максимальное количество элементов в ОЗУ при сортировке устанавливается в конфиге (пример есть в [`config/config.toml`](config/config.toml)):

```toml
max_ram_elements = 100 # Максимальное количество элементов в ОЗУ

[latencies]
shift = '0.1us' # Задержка на сдвиг ленты на одну позицию
rewind = '0.15ms' # Задержка на перемотку ленты (в начало или конец)
write = '10us' # Задержка на запись в ленту
read = '10us' # Задержка на чтение с ленты
```

### Файлы ленты

Данные для ленты в файле представляются в текстовом виде, каждое число с новой строки (примеры есть в [`tests/data`](tests/data)):

```
234
54323
4234
1231
...
```

## Алгоритм сортировки

В качестве алгоритма сортировки реализован упрощённый вариант алгоритма [TimSort](https://en.wikipedia.org/wiki/Timsort). Дополнительно для сортировки используется две временные ленты, которые нужны для слияния массивов. Также "первичная сортировка" просиходит в ОЗУ (с учётом ограничения).

## Сборка

Сборка стандартная, как и для большинства cmake проектов. Зависимости подключаются как submodules, поэтому желательно клонировать с флагом `--recursive`. 
Этого можно не делать, тогда при сборке cmake самостоятельно подтянет нужные зависимости.

```bash
git clone --recursive https://github.com/R3dKar/tape-sorter.git
cd tape-sorter
mkdir build
cmake -S . -B build
cmake --build build
```

## Запуск тестов

После сборки:

```bash
ctest --test-dir build
```
