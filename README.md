<p align='center'>
  <img width='200px' src='logo.png' style='display: block;'/>
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
- [x] JSX support
- [x] Importing JSON
- [x] Removing dead code (Tree shaking)
- [x] Supports package.json
- [x] Importing _anything_, even when it's not exported, no matter how deeply nested it is

### Planned Features
- [ ] Plugin support
- [ ] TypeScript support
- [ ] Loading CSS
- [ ] Lazy imports
- [ ] Better formatted output. (kind of ugly right now)

## Example
> Below is an example of some code that is **100%** supported.  
> (Without any plugins or configuration)
<img width='400px' src='img/code.png' style='display: block;'/>

## Installing
### Dependencies
> To build `fjb` you will need the following:
1. A C compiler (I would suggest gcc)
2. Git

### Steps to Install
#### Compiling it
1. Clone down the repository
2. Run:
```bash
./build.sh
```
> This step will generate a `fjb.out` executable file.  
> You can start using this right away.

#### Installing globally
> To continue and install `fjb` globally:
Run:
```bash
make install
```
> Also make sure you add `$HOME/.local/bin`
> to your PATH.  
> [https://unix.stackexchange.com/questions/26047/how-to-correctly-add-a-path-to-path](https://unix.stackexchange.com/questions/26047/how-to-correctly-add-a-path-to-path)

## Questions and Answers
### How serious is this?
> Not sure, it's just a hobby project at this point.

### Why are you making this?
> Frustration, frustration with existing tools.  
> The existing alternatives are not fast enough, and gets in the
> way a bit more than they should.

### Is it mature enough to use it with React?
> At some point, I want this project to get there.  
> But right now, it lacks some features.  
> For example, fjb currently supports JSX; but react-jsx is not implemented  
> right now.

### Should I use it?
> I will not encourage anyone to use it, but feel free to do it.

### Can I contribute?
> Yes you can. Simply fork it and start making pull-requests.

### How do I create a plugin for it?
> It's not possible at this point.  
> This is a feature I plan to implement.  
> If you want some other behaviour right now, simply just change the
> source code.
