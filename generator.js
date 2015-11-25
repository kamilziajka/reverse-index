'use strict';

let fs = require('fs');

const DIRECTORY = './files';

let randomGeneratorFactory = (name, chars) => ({
  name,
  generator: (stream) => {
    for (let i = 0; i < 1000; i++) {
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

files.push(randomGeneratorFactory('abc1.txt', 'abc'));
files.push(randomGeneratorFactory('abc2.txt', 'abc'));
files.push(randomGeneratorFactory('abc3.txt', 'abc'));

files.push(randomGeneratorFactory('abcde1.txt', 'abcde'));
files.push(randomGeneratorFactory('abcde2.txt', 'abcde'));
files.push(randomGeneratorFactory('abcde3.txt', 'abcde'));

files.push(randomGeneratorFactory('abcdefghijk1.txt', 'abcdefghijk'));
files.push(randomGeneratorFactory('abcdefghijk2.txt', 'abcdefghijk'));
files.push(randomGeneratorFactory('abcdefghijk3.txt', 'abcdefghijk'));
files.push(randomGeneratorFactory('abcdefghijk4.txt', 'abcdefghijk'));
files.push(randomGeneratorFactory('abcdefghijk5.txt', 'abcdefghijk'));

files.push(randomGeneratorFactory('test0.txt', 'abcdefghijlmnopqrstuvwxyz'));
files.push(randomGeneratorFactory('test1.txt', 'abcdefghijlmnopqrstuvwxyz'));
files.push(randomGeneratorFactory('test2.txt', 'abcdefghijlmnopqrstuvwxyz'));
files.push(randomGeneratorFactory('test3.txt', 'abcdefghijlmnopqrstuvwxyz'));
files.push(randomGeneratorFactory('test4.txt', 'abcdefghijlmnopqrstuvwxyz'));
files.push(randomGeneratorFactory('test5.txt', 'abcdefghijlmnopqrstuvwxyz'));
files.push(randomGeneratorFactory('test6.txt', 'abcdefghijlmnopqrstuvwxyz'));
files.push(randomGeneratorFactory('test7.txt', 'abcdefghijlmnopqrstuvwxyz'));
files.push(randomGeneratorFactory('test8.txt', 'abcdefghijlmnopqrstuvwxyz'));
files.push(randomGeneratorFactory('test9.txt', 'abcdefghijlmnopqrstuvwxyz'));

files.forEach(file => {
  let stream = fs.createWriteStream(`${DIRECTORY}/${file.name}`, { flags: 'w' });
  file.generator(stream);
});
