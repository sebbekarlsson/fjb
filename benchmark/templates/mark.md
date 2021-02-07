# Benchmarks
> These benchmarks were generated at **{{ now }}**
{% macro renderRun(run) %}
<details>
  <summary>{{ run.title }} - time: {{ run.time }}, output size: {{ run.size }} bytes</summary>

#### Output size
> The size of the output is:
```
{{ run.size }} bytes
```
#### Time
> The command:
```
{{ run.cmd }}
````
> finished in:
```
{{ run.time }}
```
</details>
{% endmacro %}



{% for mark in marks %}
<details>
  <summary>{{ mark.title }}</summary>

## What's being executed
> Code executed:
```tsx
{{ mark.source }}
```
![graphs/{{ mark.graph }}](graphs/{{ mark.graph }})

{% for run in mark.runs %}
  {{ renderRun(run) }}
{% endfor %}

#### Executed on
```
{{ mark.sysinfo }}
```
</details>
{% endfor %}
