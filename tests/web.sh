if $(test -f "/usr/bin/python3.9"); then
  PY="/usr/bin/python3.9";
elif $(test -f "/usr/bin/python3.8"); then
  PY="/usr/bin/python3.8";
else
  PY="/usr/bin/python";
fi

echo "USING PYTHON: $PY"

virtualenv -p $PY ./venv
source ./venv/bin/activate
pip install selenium
./venv/bin/python ./web.py
