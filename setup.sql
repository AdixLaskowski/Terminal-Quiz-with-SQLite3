-- Tworzenie tabeli z pytaniami
CREATE TABLE IF NOT EXISTS questions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    question TEXT NOT NULL,
    option_a TEXT NOT NULL,
    option_b TEXT NOT NULL,
    option_c TEXT NOT NULL,
    option_d TEXT NOT NULL,
    correct_option TEXT NOT NULL CHECK (correct_option IN ('A', 'B', 'C', 'D'))
);

-- Dodanie przykładowych pytań
INSERT INTO questions (question, option_a, option_b, option_c, option_d, correct_option)
VALUES
('What is the capital of France?', 'London', 'Berlin', 'Paris', 'Madrid', 'C'),
('What is 2 + 2?', '3', '4', '5', '6', 'B'),
('When did Joseph Stalin die?', '03.1945', '03.1953', '06.1953', '06.1948', 'B'),
('What is the longest river in Poland?', 'Warta', 'Bug', 'Vistula', 'Dniepr', 'C'),
('What is the country that does NOT border with Germany?', 'Austria', 'Luxembourg', 'France', 'Italy', 'D'),
('What alphabet is in use in UK?', 'Latin', 'English', 'Arabic', 'Cyrylic', 'A'),
('What does CPU stands for?', 'Central Processing Unit', 'Central Procesor Unit', 'Cental Power Unit', 'Cyan Paint Usage', 'A'),
('Who was the president of Poland in 2015-2025?', 'Andrzej Duda', 'Rafał Trzaskowski', 'Bronisław Komorowski', 'George Washington', 'A'),
('On which continent is Iran located?', 'Europe', 'Asia', 'Australia', 'South America', 'B'),
('Who was the god of oceans in greek mitology?', 'Hera', 'Zeus', 'Poseidon', 'Hades', 'C');

