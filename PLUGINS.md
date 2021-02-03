# Writing Plugins
> In this guide you will learn how to write plugins for FJB.

## How plugins works
> First of all, plugins are written in `C`,
> and they should be compiled to shared object files (`.so`).  

> The FJB plugin engine is built upon the idea of hooks.  
> Within your plugin, you can hook into different parts of the bundler.
> With these hooks, you can change or modify the behaviour of FJB.

## Hooks
> A hook that you write is simply a function, it takes in some arguments;  
> one of these arguments are `void* ptr` ( more on this later )  
> Here is a list of the currently available hooks
<details>
  <summary>HOOK_BEFORE_EVAL</summary>

> This hook is called _before_ an AST node has been evaluated.  
> Here, you can modify AST nodes before FJB evaluates them.  
> The type of `*ptr` is `AST_T*`.
</details>
<details>
  <summary>HOOK_AFTER_EVAL</summary>

> This hook is called _after_ an AST node has been evaluated.  
> This is a great hook to for example, replacing an AST node with your own.   
> The type of `*ptr` is `AST_T*`.
</details>
<details>
  <summary>HOOK_BEFORE_GENERATE</summary>

> This hook is called _before_ FJB turns an AST node into actual Javascript code.  
> Here, you can modify AST nodes before they turn into Javascript.  
> The type of `*ptr` is `AST_T*`.
</details>
<details>
  <summary>HOOK_AFTER_GENERATE</summary>

> This hook is called _after_ FJB turns an AST node into actual Javascript code.  
> This is a great hook to for example, change or customize the Javascript output.  
> The type of `*ptr` is `char*`.
</details>
<details>
  <summary>HOOK_BEFORE_COMPILE</summary>

> This hook is being called _before_ FJB generates any output.  
> This is a great hook to for example, write your own loader for some file extension.  
> The type of `*ptr` is `compiler_result_T*`.
</details>

### Writing a hook
> Below is an example of a plugin that converts javascript functions to arrow-functions.  

> **plugin.c**  
```C
#include <fjb/AST.h>
#include <fjb/plugin.h>
#include <stdio.h>
#include <string.h>

/**
 * Converts normal functions to arrow functions
 */
AST_T* function_to_arrow_function(AST_T* ast)
{
  if (ast->type != AST_FUNCTION)
    return ast;
  printf("hello\n");

  AST_T* assignment = init_ast(AST_ASSIGNMENT);
  AST_T* arrow_function = init_ast(AST_ARROW_DEFINITION);
  arrow_function->body = ast->body && ast->body->list_value->size ? ast->body : 0;

  if (!arrow_function->body) {
    arrow_function->body = init_ast(AST_NAME);
    arrow_function->body->name = strdup("null");
  }

  arrow_function->list_value = ast->list_value;
  assignment->left = init_ast(AST_NAME);

  assignment->value = arrow_function;

  if (ast->name) {
    assignment->left->name = strdup(ast->name);
  }

  return assignment;
}

/**
 * Entry point of plugin,
 * needs to be named "hook"
 */
void* hook(int type, void* ptr, fjb_env_T* env)
{
  switch (type) {
    case HOOK_AFTER_EVAL: return function_to_arrow_function((AST_T*)ptr); break;
    default: return ptr; break;
  }

  return ptr;
}

```
> The entry point of your plugin needs to be a function named `hook`.
> One general rule here is that your `hook` function, always needs to return
> a pointer which is of the same type as the `ptr` argument.  

> This hook function takes in `3` arguments:

#### int type - _the HOOK type (read more [here](#hooks))_
> You can use this integer to decide when your hook will be triggered.

#### void *ptr - _relevant data to be used in your hook_
> This pointer contains different data depending on the `int type`.  
> Read more [here](#hooks) to see what this pointer will hold for each hook type.

### fjb_env_T* env - _global FJB environment_
> This pointer contains information about the current frame of the FJB environment.  
> Such as:
- char* source
- char* filepath
- ... and more
> You can see exactly what it contains [here](./src/include/env.h)

## Compiling your plugin
> To compile your plugin, you need to link it with `libfjb.a`.  
> You should already have this file if you have compiled and installed FJB.  
> Here is an example of a `Makefile` to compile a plugin:
```make
sources = $(wildcard *.c)
objects = $(sources:.c=.o)
flags = -I$$HOME/.local/include -g -Wall -lm -ldl -fPIC -rdynamic -L$$HOME/.local/lib -lfjb
LPATH = $(HOME)/.local


libmyplugin.so: $(objects)
	gcc $(flags) -o $@ $^ -shared

%.o: %.c
	gcc -c $(flags) $< -o $@

lint:
	clang-tidy src/*.c src/include/*.h
```
> Running `make` with this `Makefile` will generate a `libmyplugin.so`.

## Using your plugin
> As of now, plugins are to be used locally within projects.  
> To use a plugin in your project, simply create a `.plugins` directory  
> within the root of your project.  
> And then, you can put all of your `.so` files inside this directory.  
> FJB will load all plugins located in this directory.  
> To be clear, the `.plugins/` directory needs to be located from where you run the FJB bundler in your project.  
> Here is an example of what this would look like in practice:
```
myapp
├── components
│   ├── app.jsx
│   ├── card.jsx
├── index.html
├── index.jsx
├── .plugins
│   └── libmyplugin.so
```
> The `.plugins/` directory can contain as many plugins (`.so` files) as you wish.

---

> For further information, please see [plugin_example](./plugin_example)
