#!/bin/sh
# КПІЗ, варіант 19: збирання всіх частин без попереджень і регресійний прогін тестів.
# Запуск (Git Bash / MSYS2):  sh build_and_test.sh
set -e
cd "$(dirname "$0")"
CFLAGS="-std=c11 -Wall -Wextra -pedantic -g"
mkdir -p build reports
OUT=reports/regression.txt
: > "$OUT"
log() { echo "$*" | tee -a "$OUT"; }

log "=== Регресійне тестування КПІЗ (варіант 19), $(date '+%d.%m.%Y %H:%M') ==="
log "Компілятор: $(gcc --version | head -n 1)"
log "Ключі: $CFLAGS"
log ""
log "--- 1. Компіляція (будь-яке попередження вважається помилкою: -Werror) ---"
gcc $CFLAGS -Werror -o build/part2.exe part2/main.c part2/func.c -lm            && log "part2: OK"
gcc $CFLAGS -Werror -o build/part3.exe part3/main.c part3/matrix.c part3/sort.c part3/search.c && log "part3: OK"
gcc $CFLAGS -Werror -o build/part4.exe part4/main.c part4/text.c                && log "part4: OK"
gcc $CFLAGS -Werror -o build/part5.exe part5/main.c part5/bus.c part5/storage.c && log "part5: OK"
gcc $CFLAGS -Werror -o build/test_assert.exe tests/test_assert.c part2/func.c part3/matrix.c part3/sort.c part3/search.c part4/text.c -lm && log "test_assert: OK"
gcc $CFLAGS -Werror -o build/test_cunit.exe tests/test_cunit.c part3/matrix.c part3/sort.c part3/search.c part4/text.c -lcunit && log "test_cunit: OK"
gcc $CFLAGS -Werror -fstack-protector-all -o build/test_tdd.exe tests/test_tdd.c part4/text.c && log "test_tdd: OK"
log ""
log "--- 2. Модульні тести (assert) ---"
./build/test_assert.exe | tee -a "$OUT"
log ""
log "--- 3. Модульні тести (CUnit) ---"
./build/test_cunit.exe | tail -n 6 | tee -a "$OUT"
log ""
log "--- 4. Тести TDD (is_anagram) ---"
./build/test_tdd.exe | tee -a "$OUT"
log ""
log "--- 5. Перевірка пам'яті (memtrack) ---"
gcc $CFLAGS -c common/memtrack.c -o build/memtrack.o
gcc $CFLAGS -DMEMTRACK -include common/memtrack.h -o build/tests_mt.exe tests/test_assert.c part2/func.c part3/matrix.c part3/sort.c part3/search.c part4/text.c build/memtrack.o -lm
./build/tests_mt.exe > /dev/null 2> build/mt.txt; tee -a "$OUT" < build/mt.txt
log ""
log "--- 6. Статичний аналіз (cppcheck) ---"
if command -v cppcheck > /dev/null; then
    cppcheck --enable=warning,style,performance,portability --std=c11 --inline-suppr -q part2 part3 part4 part5 tests 2> build/cpp.txt || true
    if [ -s build/cpp.txt ]; then tee -a "$OUT" < build/cpp.txt; else log "cppcheck: зауважень немає"; fi
else
    log "cppcheck не встановлено"
fi
log ""
log "=== Регресійне тестування завершено успішно ==="
