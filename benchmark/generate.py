import subprocess
import jinja2
import os
import datetime
import json
import matplotlib.pyplot as plt
import base64
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument(
    "--one", type=bool, default=False, help="Runs only one benchmark")
args = parser.parse_args()


PWD = os.getcwd()

TMPL_PATH = PWD + '/benchmark/templates'

jinja_env = jinja2.Environment(loader=jinja2.FileSystemLoader(TMPL_PATH))

ESBUILD_PATH = "/benchmark/node_modules/.bin/esbuild"
ESBUILD_ARGS = ' --bundle --outfile=./dist.js'
PARCEL_PATH = '/benchmark/node_modules/.bin/parcel build'
PARCEL_DIST = '/dist/index.js'
PARCEL_ARGS = ''
POI_PATH = '/benchmark/node_modules/.bin/poi'
POI_DIST = '/dist/assets/js/index.js'
POI_ARGS = ''
FJB_PATH = "/fjb.out"
FJB_ARGS = ' > ./dist.js'

DUMP_NAME = PWD + '/bench.json'
GRAPH_DIR = PWD + '/graphs'

OUTPUT_DOCUMENTS = [
    ('mark.md', PWD + '/benchmarks.md'),
    ('mark.html', PWD + '/benchmarks.html')
]

if not os.path.isdir(GRAPH_DIR):
    os.mkdir(GRAPH_DIR)

RUNS = [
    {
        'title': 'FJB',
        'exec': PWD + FJB_PATH,
        "args": FJB_ARGS
    },
    {
        'title': 'esbuild',
        'exec': PWD + ESBUILD_PATH,
        "args": ESBUILD_ARGS
    },
    {
        'title': 'parcel',
        'exec': PWD + PARCEL_PATH,
        "args": PARCEL_ARGS,
        'dist': PARCEL_DIST
    },
    {
        'title': 'poi',
        'exec': PWD + POI_PATH,
        "args": POI_ARGS,
        'dist': POI_DIST
    },
]


if args.one:
    RUNS = [RUNS[0]]


def create_mark(title, path, runs=RUNS):
    return {"title": title, "path": PWD + path, "runs": runs or RUNS}


BENCHMARKS = [
    create_mark(
        "Ternary operator", '/tests/src/test_projects/ternary/index.js'
    ),
    create_mark("Destruct", '/tests/src/test_projects/destruct/index.js'),
    create_mark(
        "Importing ES6 modules", '/tests/src/test_projects/es6/index.js'
    ),
    create_mark(
        "Importing aliased ES6 modules",
        '/tests/src/test_projects/alias_imports/index.js'
    ),
    create_mark(
        "Importing CSS", '/tests/src/test_projects/css_import/index.js'
    ),
    create_mark(
        "Importing JSON", '/tests/src/test_projects/json_import/index.js'
    ),
    create_mark(
        "Importing `ceil` from lodash, with tree shaking.",
        '/tests/src/test_projects/with_lodash/index.js'
    ),
    create_mark(
        "Require `jquery`", '/tests/src/test_projects/with_jquery/index.js'
    ),
    create_mark(
        "Importing `react` from react",
        '/tests/src/test_projects/react_import/index.jsx'
    ),
    create_mark(
        "React project with JSX",
        '/tests/src/test_projects/with_react/index.jsx'
    )
]


def get_sysinfo():
    process = subprocess.Popen("lscpu", stdout=subprocess.PIPE, shell=True)
    return process.communicate()[0].decode()


def get_filesize(filename):
    return os.path.getsize(filename)


def run_entry(mark, entry):
    timebefore = datetime.datetime.now()

    print(entry)

    cmd = "{exe} {path} {args}".format(
        exe=entry['exec'], path=mark['path'], args=entry['args']
    )

    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    process.communicate()

    diff = datetime.datetime.now() - timebefore

    return {
        'time': diff,
        'size': get_filesize(PWD + entry.get('dist', '/dist.js')),
        'cmd': cmd,
        **entry
    }


def run_benchmark(mark):
    return list(map(lambda x: run_entry(mark, x), mark['runs']))


def run_benchmarks(marks):
    return map(
        lambda x: dict(
            list(x.items()) + list(
                dict(
                    runs=list(
                        sorted(run_benchmark(x), key=lambda x: x['time'])),
                    source=open(x['path']).read(),
                    sysinfo=get_sysinfo(),
                ).items()
            )
        ), marks
    )


def generate_graph(mark):
    markname = base64.b64encode(mark['title'].encode()).decode('utf-8')
    filename = '{}.png'.format(markname)

    x = []
    y = []
    y2 = []

    for r in mark['runs']:
        x.append(r['title'])
        y.append(str(r['time']))
        y2.append(r['size'])

    plt.figure()
    plt.subplot(1, 2, 1)
    plt.ylabel('time')
    plt.bar(x, y)
    plt.subplot(1, 2, 2)
    plt.ylabel('size (bytes)')
    plt.bar(x, y2)

    plt.tight_layout()
    plt.savefig('{}/{}'.format(GRAPH_DIR, filename), dpi=99)

    return filename


def generate_graphs(marks):
    return list(map(generate_graph, marks))


def set_graphname(mark, name):
    mark['graph'] = name
    return mark


def generate(documents, results):
    marks = list(results)
    graphs = list(generate_graphs(marks))
    marks = [set_graphname(m, graphs[i]) for i, m in enumerate(marks)]

    for document in documents:
        document_input, document_output = document

        print('Generating {} ...'.format(document_output))
        template = jinja_env.get_template(document_input)
        print('Done')

        open(DUMP_NAME,
             'w+').write(
                 json.dumps(marks, default=str, sort_keys=True, indent=2))
        contents = template.render(
            marks=marks, now=str(datetime.datetime.now()))
        open(document_output, 'w+').write(contents)


def run():
    generate(OUTPUT_DOCUMENTS, run_benchmarks(BENCHMARKS))


if __name__ == '__main__':
    run()
