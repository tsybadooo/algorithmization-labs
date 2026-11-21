# run.sh: run EXE with INPUT, print a console-like transcript (input echoed)
# usage: run.sh exe "input lines"
exe="$1"; inp="$2"
echo "> $exe"
[ -n "$inp" ] && printf '%s\n' "$inp" | sed 's/^/[ввід] /'
printf '%s\n' "$inp" | "./$exe" 2>&1
echo "[код завершення: $?]"
echo
