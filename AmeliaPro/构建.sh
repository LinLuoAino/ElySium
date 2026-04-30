#!/system/bin/sh

FILTER_WORDS="此NDK完全免费 官方TG频道 Bilibili: ManagerBasic"

/data/user/0/aidepro.top/no_backup/ndksupport-1710240003/android-ndk-aide/ndk-build 2>&1 | while IFS= read line; do
    skip=0
    
    for word in $FILTER_WORDS; do
        if echo "$line" | grep -q "$word"; then
            skip=1
            break
        fi
    done
    
    if echo "$line" | grep -q "^------------------------------------------------$"; then
        skip=1
    fi
    
    if echo "$line" | grep -q "^|_$"; then
        skip=1
    fi
    
    if [ "$skip" = "0" ]; then
        echo "$line"
    fi
done