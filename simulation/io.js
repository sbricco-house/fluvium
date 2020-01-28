const readline = require('readline');

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
});

function questionPromise(question) {
    return new Promise((resolve, err) => {
        rl.question(question, answer => resolve(answer))
    })
}

module.exports.question = questionPromise
module.exports.readLine = rl