if [  $1!='' ]
then
    msg=$1;
else
    exit 1
fi

if [  $2!='' ]
then
    brach=$2;
else
    brach="master"; 
fi

git add -u
git commit -m $msg
git push  origin $brach
