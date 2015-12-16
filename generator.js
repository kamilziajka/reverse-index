'use strict';

let fs = require('fs');

const DIRECTORY = './files';
const WORDS_PER_FILE = 200;
const FILES_PER_TYPE = 200;

let randomGeneratorFactory = (name, chars) => ({
  name,
  generator: (stream) => {
    for (let i = 0; i < WORDS_PER_FILE; i++) {
      let word = '';

      while (word.length < 5) {
        word += chars[Math.floor((Math.random() * 100) % chars.length)];
      }

      word += ' ';

      stream.write(word);
    }

    stream.end();
  }
});

let files = [];

for (let i = 0; i < FILES_PER_TYPE; i++) {
  files.push(randomGeneratorFactory(`abc${i}.txt`, 'abc'));
}

for (let i = 0; i < FILES_PER_TYPE; i++) {
  files.push(randomGeneratorFactory(`abcde${i}.txt`, 'abcde'));
}

for (let i = 0; i < FILES_PER_TYPE; i++) {
  files.push(randomGeneratorFactory(`abcdefghijk${i}.txt`, 'abcdefghijk'));
}

for (let i = 0; i < FILES_PER_TYPE; i++) {
  files.push(randomGeneratorFactory(`test${i}.txt`, 'abcdefghijlmnopqrstuvwxyz'));
}

files.forEach(file => {
  let stream = fs.createWriteStream(`${DIRECTORY}/${file.name}`, { flags: 'w' });
  file.generator(stream);
});
