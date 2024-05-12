import fs from 'fs'
import path from 'path'
import clipboardy from 'clipboardy'

const merged: string[] = []

const merge = (filepath: string) => {
    let result = ''
    const filename = path.basename(filepath)
    if (merged.includes(filename)) {
        return result
    }
    merged.push(filename)
    for (const line of fs.readFileSync(filepath).toString().split('\n')) {
        if (line.startsWith('#pragma')) {
            continue
        }
        if (line.startsWith('#include')) {
            const left = line.indexOf('"')
            if (left != -1) {
                const right = line.lastIndexOf('"')
                result += merge(path.join(path.dirname(filepath), line.slice(left + 1, right)))
                continue
            }
        }
        result += line + '\n'
    }
    return result
}

const comment = `
// The following code is generated by the merge tool.
// https://github.com/MirionQs/Concrete
`

clipboardy.writeSync(comment + merge('../main.cpp'))