if [  $1!='' ]
then
   msg=$1;
else
    msg="bug"; 
   echo "bug";
   return
fi

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
git add tnos/*.c
git add tnos/*.h

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
git push  origin master
