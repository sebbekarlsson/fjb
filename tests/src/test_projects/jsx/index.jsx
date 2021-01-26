function render(firstname, lastname) {
  return (
    <div title="hello" data-name={lastname}>
      <h1>Welcome {firstname}</h1>
      <p>
        Hello {firstname} {lastname}
      </p>
    </div>
  );
}

document.addEventListener('DOMContentLoaded', function () {
  document.getElementById("root").appendChild(render("John", "Doe"));
});
