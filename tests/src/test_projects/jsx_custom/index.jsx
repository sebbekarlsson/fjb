function HeadTitle(title) {
  return <h1>{title}</h1>;
}

function Text(text) {
  return <p>{text}</p>;
}

function Section(title, text) {
  return (
    <section>
      <HeadTitle title={title} />
      <Text text={text} />
    </section>
  );
}

function App() {
  return Section("Hello", "welcome to my website");
}

document.getElementById("root").appendChild(App());
