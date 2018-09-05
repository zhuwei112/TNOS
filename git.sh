if [  $1!='' ]
then
    msg=$1;
else
    msg="bug"; 
fi

if [  $2!='' ]
then
    brach=$2;
else
    brach="master"; 
fi

git add README_Chinese.md
git add README.md
git add git.sh
git add board
git add board/stm32_f10x
git add board/stm32_f10x/*.h
git add board/stm32_f10x/*.c
git add board/stm32_f10x/*.S

git add board/stm32_f10x/Libraries
git add board/stm32_f10x/Libraries/*

git add board/stm32_f10x/Project
git add board/stm32_f10x/Project/RVMDK_5
git add board/stm32_f10x/Project/RVMDK_5/tnos.uvoptx
git add board/stm32_f10x/Project/RVMDK_5/tnos.uvprojx

git add tnos
git add tnos/tnos_cfg.h
git add tnos/tnos_cpu_c.h
git add tnos/tnos_def.h
git add tnos/tnos_dev.h
git add tnos/tnos_test.c

git add tnos/lib_cpu
git add tnos/lib_cpu/arm
git add tnos/lib_cpu/arm/cortex-m3
git add tnos/lib_cpu/arm/cortex-m3/*.c
git add tnos/lib_cpu/arm/cortex-m3/*.h
git add tnos/lib_cpu/arm/cortex-m3/*.S

git add tnos/souce
git add tnos/souce/*.c
git add tnos/souce/*.h

git add tnos/utility
git add tnos/utility/*.c
git add tnos/utility/*.h


git commit -m '$msg'
git push  origin '$brach'
