echo $(cat pkg.json | grep version | tr -d \ | tr -d \" | tr \: \\n | tr -d \\n | tr -d "version" | tr -d \,)
