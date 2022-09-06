while : ; do
  ./UnixWriteCrossPage
  if [ $? -ge 3 ]; then
      break
  fi
done