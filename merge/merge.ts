import fs from 'fs'
import clipboardy from 'clipboardy'

let result = ''

const merge = (path: string) => {
    for (const line of fs.readFileSync(path).toString().split('\n')) {
        if (line.startsWith('#pragma')) {
            continue
        }
        if (line.startsWith('#include')) {
            const left = line.indexOf('"')
            if (left != -1) {
                const right = line.lastIndexOf('"')
                result += merge('../' + line.slice(left + 1, right))
                continue
            }
        }
        result += line + '\n'
    }
    return result
}

clipboardy.writeSync(merge('../main.cpp'))