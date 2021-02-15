export interface Book<T> {
  title: string;
  description: T
};

const book:Book<string> = { title: "Harry Potter", description: "book about magic" };
