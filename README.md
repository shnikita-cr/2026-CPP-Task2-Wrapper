# Лабораторная работа №2: Wrapper / Engine

## Описание задания

Необходимо реализовать архитектурное решение на языке C++, позволяющее инкапсулировать объект произвольного класса, один из его методов и словарь аргументов со значениями по умолчанию. 

Последующий вызов инкапсулированного метода должен выполняться через управляющий класс (`Engine`) по назначенному строковому алиасу (имени команды). При вызове должна быть обеспечена возможность передачи новых значений аргументов, которые частично или полностью заменяют значения по умолчанию.

### Пример ожидаемого использования:

```c++
Wrapper wrapper(&subj, &Subject::func, {{"arg1", 0}, {"arg2", 0}});
Engine engine;
engine.register_command(&wrapper, "command_name"); 
std::cout << engine.execute("command_name", {{"arg1", 4}, {"arg2", 5}}) << std::endl;
```

---

## Архитектурные требования и демонстрация

Разработанные классы `Wrapper` и `Engine` должны быть применены для решения задачи модификации текстового документа. 

В качестве базового типа аргументов в словаре необходимо использовать `std::string` (либо обеспечить полностью обобщенную шаблонную сигнатуру). Возвращаемое значение метода `execute` может быть строкой `std::string` или выводиться автоматически (быть шаблонным).

В рамках демонстрации работоспособности архитектуры необходимо реализовать класс-обработчик текста, методы которого будут обернуты во `Wrapper`, для выполнения следующих операций:
* **Замена слов** в тексте.
* **Поиск словосочетаний** (возвращает статус: найдено / не найдено).
* **Изменение первых букв слов на заглавные** (капитализация каждого слова).
* **Изменение первой буквы предложения на заглавную**.
* **Добавление последовательной нумерации к каждому предложению** (при этом каждое предложение должно начинаться с новой строки).

---

## Правила и формат сдачи работы

1. Перед началом работы необходимо сделать ответвление (fork) данного репозитория в личный аккаунт GitHub.
2. Создайте ветку `dev` в вашем ответвлении и ведите разработку в ней (`git checkout -b dev`).
3. Весь исходный код должен быть локализован в директории `src/` (например, `src/main.cpp`). Конфигурация сборки должна управляться корневым файлом `CMakeLists.txt`.
4. По окончании выполнения задания создайте PR (Pull Request) из ветки `dev` вашего ответвления в ветку `main` этого репозитория.
5. **Название PR должно строго содержать ваши Фамилию и Имя**.
6. После создания PR назначьте в качестве рецензента проверяющего преподавателя: **`shnikita-cr`**.

---

## Автоматическая проверка

При создании или обновлении PR автоматически запускается комплекс тестов на сервере GitHub Actions. При ошибке их выполнения решение задания принято не будет.

Конвейер автоматизации состоит из трёх этапов проверки:
1. **Сборка проекта (Release Mode):** Проверяет корректность конфигурации CMake и компиляцию проекта в режиме оптимизации. Сборка идёт с `-Wall -Wextra -Werror`: любое предупреждение компилятора считается ошибкой.
2. **Статический анализ кода:** Проверяет исходный код на соответствие стандартам качества и сложность:
   * `lizard` — вычисляет цикломатическую сложность функций (максимально допустимая сложность — **18**).
   * `cppcheck` — `warning,performance,portability`, inline-подавления, `unusedFunction` подавлен.
   * `clang-tidy` — проверяет код на соответствие современным стандартам (Modern C++), стилю оформления и производительности.
3. **Динамический анализ памяти (ASan/UBSan):** Запускает все собранные исполняемые файлы в режиме отладки со встроенными инструментами анализа во время выполнения:
   * `AddressSanitizer` — отслеживает выходы за границы массивов, работу с освобожденной памятью и утечки памяти.
   * `UndefinedBehaviorSanitizer` — фиксирует неопределенное поведение (переполнения, деления на ноль и аналогичные нарушения).

---

## Локальная проверка перед отправкой

### 1. Сборка с предупреждениями
```bash
cmake -B build-sanitizers -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_FLAGS="-Wall -Wextra -Werror"
cmake --build build-sanitizers --config Debug
```

### 2. Запуск статических анализаторов

* **Для Linux / macOS (в терминалах Bash/Zsh):**
  ```bash
  lizard src/ -C 18 && \
  cppcheck --enable=warning,performance,portability --std=c++17 --language=c++ --inline-suppr --suppress=missingIncludeSystem --suppress=unusedFunction --quiet --error-exitcode=1 src/ && \
  find src/ -name "*.cpp" | xargs clang-tidy -p build-sanitizers/ --checks='-*,bugprone-*,modernize-*,performance-*,readability-*,-modernize-use-trailing-return-type' --warnings-as-errors='*'
  ```

* **Для Windows (в консоли PowerShell):**
  ```powershell
  lizard src/ -C 18
  if ($?) {
      cppcheck --enable=warning,performance,portability --std=c++17 --language=c++ --inline-suppr --suppress=missingIncludeSystem --suppress=unusedFunction --quiet --error-exitcode=1 src/
  }
  if ($?) {
      Get-ChildItem src/ -Filter *.cpp -Recurse | ForEach-Object {
          clang-tidy $_.FullName -p build-sanitizers/ --checks='-*,bugprone-*,modernize-*,performance-*,readability-*,-modernize-use-trailing-return-type' --warnings-as-errors='*'
      }
  }
  ```

### 3. Локальная проверка динамической памяти (ASan/UBSan)
```bash
cmake -B build-sanitizers -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build-sanitizers --config Debug

find build-sanitizers -type f -perm -u+x ! -path '*/CMakeFiles/*' ! -name '*.so*' -exec timeout 60 {} \;
```
