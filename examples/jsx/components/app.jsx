import { Card } from "./card";
import cardData from "../data/data.json";

const appStyle = `
  width: 100%;
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 0.1fr));
  grid-template-rows: repeat(auto-fit, minmax(200px, 0.1fr));
  grid-gap: 2rem;
`;

export function App() {
  return (
    <div style={appStyle}>
      {cardData.map((data) => (
        <Card text={data.title} description={data.description} />
      ))}
    </div>
  );
}
