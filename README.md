<p align='center'>
  <img src='logo.png' style='display: block;'/>
</p>

<p align='center' style='text-align: center;'>
  <h1>Fast Javascript Bundler</h1>
</p>

## What?
> It is what it says it is.  
> However, this bundler aims to be a monolithic does-it-all type of bundler;
> with zero configuration needed.

## How does one use it?
```bash
fjb index.js
```

### Features
- [x] Being fast
- [x] ES6 module imports / exports
- [x] Aliased imports
- [x] Wildcard imports
- [X] JSX support
- [x] Removing dead code (Tree shaking)
- [x] Supports package.json
- [x] Importing _anything_, even when it's not exported, no matter how deeply nested it is

### Planned Features
- [ ] Plugin support
- [ ] TypeScript support
- [ ] Loading CSS
- [ ] Loading JSON
- [ ] Lazy imports 

## Installing
### Dependencies
> To build `fjb` you will need the following:
1. A C compiler (I would suggest gcc)
2. Git

### Steps to Install
1. Clone down the repository
2. Run:
```bash
./build.sh
```
> This step will generate a `fjb.out` executable file.  
> You can start using this right away.

> To continue and install `fjb` globally:
3. Run:
```bash
sudo make install
```

## That's it!
> Cheers.
