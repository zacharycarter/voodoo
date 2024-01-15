import { basicSetup } from "codemirror";
import Big from "big.js";
import { EditorView, keymap } from "@codemirror/view";
import { syntaxHighlighting, HighlightStyle } from "@codemirror/language";
import { defaultKeymap, indentWithTab } from "@codemirror/commands";
import { janet } from "codemirror-lang-janet";
import { dracula } from "thememirror";
import { tags } from "@lezer/highlight";

import * as Signal from "./signals.js";
import Voodoo from "../../voodoo.js";

import "../css/main.css";

function isNumberNode(node) {
  return node.type.name === "Number";
}

function alterNumber({ state, dispatch }, amount) {
  const range = state.selection.ranges[state.selection.mainIndex];
  const tree = syntaxTree(state);

  let node = tree.resolveInner(range.head, -1);
  if (!isNumberNode(node)) {
    node = tree.resolveInner(range.head, 1);
  }
  if (!isNumberNode(node)) {
    return false;
  }

  // TODO: we shouldn't be doing any floating point math; we should
  // parse this as a decimal number and increment it as a decimal number
  const numberText = state.sliceDoc(node.from, node.to);
  let number;
  try {
    number = Big(numberText);
  } catch (e) {
    console.error("unable to parse number: ", numberText);
    return false;
  }
  const decimalPointIndex = numberText.indexOf(".");
  const digitsAfterDecimalPoint =
    decimalPointIndex < 0 ? 0 : numberText.length - decimalPointIndex - 1;
  const increment = Big("10").pow(-digitsAfterDecimalPoint);

  const newNumber = number.add(amount.times(increment));
  const newNumberText = newNumber.toFixed(digitsAfterDecimalPoint);

  const lengthDifference = newNumberText.length - numberText.length;

  dispatch(
    state.update({
      changes: {
        from: node.from,
        to: node.to,
        insert: newNumberText,
      },
      selection: EditorSelection.single(node.from, node.to + lengthDifference),
      scrollIntoView: true,
      userEvent: "alterNumber",
    })
  );
  return true;
}

const highlightStyle = HighlightStyle.define([
  { tag: tags.keyword, color: "var(--pink)" },
  { tag: tags.atom, color: "var(--pink)" },
  { tag: tags.number, color: "var(--pink)" },
  { tag: tags.comment, color: "var(--comment)" },
  { tag: tags.null, color: "var(--purple)" },
  { tag: tags.bool, color: "var(--purple)" },
  { tag: tags.string, color: "var(--yellow)" },
]);

const theme = EditorView.theme({
  "&": {
    color: "var(--foreground)",
    backgroundColor: "var(--background)",
  },
  ".cm-content": {
    padding: "0",
    caretColor: "var(--foreground)",
  },
  ".cm-cursor": {
    borderLeftColor: "var(--foreground)",
  },
  ".cm-activeLine": {
    backgroundColor: "initial",
  },
  "&.cm-focused .cm-activeLine": {
    // TODO: this breaks selection highlighting, which is crazy
    // backgroundColor: 'var(--line)',
  },
  ".cm-activeLineGutter": {
    backgroundColor: "initial",
  },
  "&.cm-focused .cm-activeLineGutter": {
    backgroundColor: "var(--selection)",
  },
  ".cm-selectionMatch": {
    outline: "solid 1px var(--comment)",
    borderRadius: "2px",
    backgroundColor: "initial",
  },
  "&.cm-focused .cm-matchingBracket": {
    outline: "solid 1px var(--green)",
    borderRadius: "2px",
    color: "var(--green)",
    backgroundColor: "initial",
  },
  "&.cm-focused .cm-nonmatchingBracket": {
    outline: "solid 1px var(--red)",
    borderRadius: "2px",
    color: "var(--red)",
    backgroundColor: "initial",
  },
  // slightly subtler as you type; i dunno
  // "&.cm-focused .cm-activeLine .cm-matchingBracket": {
  //   outline: 'none',
  // },
  ".cm-foldPlaceholder": {
    outline: "solid 1px var(--comment)",
    border: "none",
    width: "2ch",
    display: "inline-block",
    margin: "0",
    padding: "0",
    textAlign: "center",
    borderRadius: "2px",
    backgroundColor: "var(--background)",
    color: "var(--comment)",
  },
  "&.cm-focused .cm-selectionBackground, .cm-selectionBackground, ::selection":
    {
      backgroundColor: "var(--selection)",
    },
  ".cm-gutters": {
    backgroundColor: "var(--foreground)",
    color: "var(--comment)",
    border: "none",
  },
  // TODO: style the "find/replace" box
});

const keyBindings = [indentWithTab, ...defaultKeymap];
const parent = document.getElementById("vd-editor");
const signals = {
  scriptDirty: Signal.create(false),
};

const Module = {
  get_signal: Signal.get,
  set_signal: Signal.set,
  signals: signals,
  preInit: () => {
    Module.FS.mkdir("/voodoo");
    Module.FS.mount(Module.FS.filesystems.IDBFS, {}, "/voodoo");
    // Module.FS.writeFile("/voodoo/game.janet", "(print \"hello world\")", { encoding: "utf8" });
  },
  preRun: [
    () => {
      const entryScriptSrc = Module.FS.readFile("/assets/scripts/game.janet", {
        encoding: "utf8",
      });
      const entryScriptSrcBuf = new TextEncoder("utf-8").encode(entryScriptSrc);
      const entryScriptFile = Module.FS.open("/voodoo/game.janet", "w+");
      Module.FS.write(
        entryScriptFile,
        entryScriptSrcBuf,
        0,
        entryScriptSrcBuf.length,
        0
      );
      Module.FS.close(entryScriptFile);

      Module.editor = new EditorView({
        extensions: [
          basicSetup,
          janet(),
          keymap.of(keyBindings),
          EditorView.updateListener.of(function (viewUpdate) {
            if (viewUpdate.docChanged) {
              Module.set_signal(Module.signals.scriptDirty, true);
              Module.FS.writeFile(
                "/voodoo/game.janet",
                viewUpdate.state.doc.toString(),
                { encoding: "utf8" }
              );
              Module.FS.syncfs(true, function (err) {
                if (!!err) console.error(err);
              });
            }
          }),
          dracula,
        ],
        parent,
        doc: entryScriptSrc,
      });
    },
  ],
};

Voodoo(Module).then((VD) => {
  let ctrlClickedAt = 0;
  const isTryingToEngageNumberDrag = () => {
    return performance.now() - ctrlClickedAt < 100;
  };

  parent.addEventListener("pointerdown", (e) => {
    if ((e.buttons === 1 || e.buttons === 2) && e.ctrlKey) {
      ctrlClickedAt = performance.now();
      parent.setPointerCapture(e.pointerId);
      e.preventDefault();
    }
  });
  parent.addEventListener("contextmenu", (e) => {
    if (isTryingToEngageNumberDrag()) {
      e.preventDefault();
    }
  });
  parent.addEventListener("pointermove", (e) => {
    if (parent.hasPointerCapture(e.pointerId)) {
      alterNumber(VD.editor, Big(e.movementX).times("1"));
    }
  });

  // There is a bug in Firefox where ctrl-click fires as
  // a pointermove event instead of a pointerdown event,
  // and then will not respect setPointerCapture() when
  // called from the pointermove event.
  //
  // https://bugzilla.mozilla.org/show_bug.cgi?id=1504210
  //
  // So on Firefox you have to use an actual right-click.
  // It's very annoying. This is an *okay* workaround.
  document.addEventListener("pointermove", (e) => {
    if (!VD.editor.hasFocus) {
      return;
    }
    if (e.shiftKey && e.metaKey) {
      alterNumber(VD.editor, Big(e.movementX).times("1"));
    }
  });

  // if (canSave) {
  //   setInterval(function () {
  //     save(editor);
  //   }, 30 * 1000);
  //   document.addEventListener("pagehide", () => {
  //     save(editor);
  //   });
  //   let savedBefore = false;
  //   // iOS Safari doesn't support beforeunload,
  //   // but it does support unload.
  //   window.addEventListener("beforeunload", () => {
  //     savedBefore = true;
  //     save(editor);
  //   });
  //   window.addEventListener("unload", () => {
  //     if (!savedBefore) {
  //       save(editor);
  //     }
  //   });
  // }
});
