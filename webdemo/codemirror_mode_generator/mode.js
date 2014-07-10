(function(mod) {
    if (typeof exports == 'object' && typeof module == 'object') // CommonJS
        mod(require('../../lib/codemirror'));
    else if (typeof define == 'function' && define.amd) // AMD
        define(['../../lib/codemirror'], mod);
    else // Plain browser env
        mod(CodeMirror);
})(function(CodeMirror) {
    'use strict';

    /* Some configuration is needed if the Equelle language is changed */
    var  LINECONT_STRING = '...'
        ,EOL_TOKEN = 'EOL';

    /* The lexer */
    var lexer = ( function() {
        var r = [
            ##lexer_regexes##
        ];
        var a = [
            ##lexer_actions##
        ];
        /* The actual lexing function */
        return function(stream, lineno) {
            /* Find all matches */
            var matches = [], lengths = [];
            for (var i = 0; i < r.length; i++) {
                var m = stream.match(r[i],false);
                if (m) {
                    matches.push(i);
                    lengths.push(m[0].length);
                }
            }
            /* Select the first longest match */
            var max = -1, maxI;
            for (var i = 0; i < matches.length; i++) {
                if (lengths[i] > max) {
                    max = lengths[i];
                    maxI = matches[i];
                }
            }
            /* If match found, use that */
            if (maxI !== undefined) {
                var m = stream.match(r[maxI]);
                return (a[maxI])(m[0],lineno);
            }
            /* None found, return nothing, this shouldn't really happen */
            return undefined;
        };
    })();

    /* The parser, keeps track of indentations */
    var parser = ( function() {
        /* The actual parsing function */
        return function(stream, state, config, token) {
            /* Keep track of whether a line contained a block starting token */
            /* ... or if it is a continued line */
            /* ... and where the last Function token was, if we continue a line after this token, we probably want to indent to the column after it */
            if (state.EOL) { state.EOL = false; state.lineContainedBlockStart = false }
            if (token.name == '{') { state.lineContainedBlockStart = true }
            if (token.name == 'LINECONT') { state.continuedLine = true }
            if (token.name == 'FUNCTION') { state.functionTokenPos = (stream.column() - stream.indentation() + 8) }

            /* At the end-of-line, or after a line continuation token, update indentation of current block so that if user indents self, it will keep going with that indent */
            /* ... unless it is a continued line, where we want to keep the indent of the parent line */
            /* ... or the line containd a block starting token, which means we are actually inside the new block */
            if (((token.name == EOL_TOKEN && !state.continuedLine) || token.name == 'LINECONT') && !state.lineContainedBlockStart) {
                state.blockIndent[state.blockLevel] = stream.indentation();
            }

            /* Reset the above keeping-track variables when we reach the enf of a line */
            if (token.name == EOL_TOKEN) { state.EOL = true; state.continuedLine = false; state.functionTokenPos = 0 }

            /* Add and remove block level indentations */
            if (token.name == '{') {
                /* Inside a new block, add a level of indentation from this lines */
                state.blockIndent.push(stream.indentation()+config.indentUnit);
                state.blockLevel += 1;
            } else if (token.name == '}') {
                /* Left a block, remove a level of indentation */
                state.blockIndent.pop();
                state.blockLevel -= 1;
            }
        };
    })();

    /* The style function, which gives a token a css-class based on its name */
    var tokenStyle = function(token) {
        if (token && token.name) switch(token.name) {
            ##style_cases##
            default: return token.name;
        }
        else return null;
    };


    /* The actual mode logic */
    CodeMirror.defineMode('equelle', function(config) {
        return {
            startState: function() { return {
                 blockLevel: 0
                ,blockIndent: [0]
                ,lineContainedBlockStart: false
                ,EOL: false
                ,continuedLine: false
                ,functionTokenPos: 0
            }}
            ,token: function token(s, state) {
                /* Run the lexer, this will return a token, and advance the stream */
                var lex;
                try { lex = lexer(s,0); }
                /* This throws error for unexpected characters, the stream has advanced past them, so mark them as error */
                catch (e) { return 'error'; }

                /* Also catch line continuation as a token */
                if (lex && lex.match && _.str.startsWith(lex.match,LINECONT_STRING)) { lex.name = 'LINECONT' }

                /* If no error, we should have gotten a token back, send this token to the parser */
                if (lex && lex.name) parser(s,state,config,lex);

                /* If we are at the end-of-line, we should also make sure that the EOL token is sent to the parser, unless the last token was a LINECONT (...) */
                if (s.eol() && lex && lex.name != EOL_TOKEN && lex.name != 'LINECONT') token(s,state);

                /* We are now ready to return the style of the last token we read */
                return tokenStyle(lex);
            }
            ,indent: function(state, after) {
                if (_.str.startsWith(after,'}')) {
                    /* The block ending token should be indented to the previous block level */
                    return state.blockIndent[state.blockLevel-1];
                } else if (state.continuedLine) {
                    /* If it is a continued line, indent to the same level as the last Function token, if any */
                    return (state.blockIndent[state.blockLevel]+state.functionTokenPos);
                } else {
                    /* Everything else should be indented as the block */
                    return state.blockIndent[state.blockLevel];
                }
            }
        };
    });
});
