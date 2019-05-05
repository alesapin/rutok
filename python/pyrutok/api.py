from rutokpy import TokenImpl, CommonInputStreamFromFD, SentenceImpl
from rutokpy import TokenHasherImpl
from rutokpy import CommonInputStreamFromString
from rutokpy import GraphemTag, SemanticTag, TokenType
from builtins import object

def _to_tag_set(tag):
    if isinstance(tag, GraphemTag):
        return [t for t in GraphemTag.to_tag_set(tag) if t != GraphemTag.UNKNOWN]
    elif isinstance(tag, SemanticTag):
        return [t for t in SemanticTag.to_tag_set(tag) if t != SemanticTag.UNKNOWN]
    elif isinstance(tag, TokenType):
        return tag
    else:
        raise Exception("Unknown tag")


GraphemTag.__str__ = lambda x: str(_to_tag_set(x))
SemanticTag.__str__ = lambda x: str(_to_tag_set(x))


class Token(object):
    def __init__(self, token_impl):
        self._token_impl = token_impl

    @staticmethod
    def concat(tokens):
        return TokenImpl.concat([t._token_impl for t in tokens])

    @staticmethod
    def upper(token):
        return TokenImpl.upper(token._token_impl)

    @staticmethod
    def lower(token):
        return TokenImpl.lower(token._token_impl)

    @staticmethod
    def title(token):
        return TokenImpl.title(token._token_impl)

    @staticmethod
    def refine(token):
        return TokenImpl.refine(token._token_impl)

    def equals_ignorecase(self, o):
        return self._token_impl.equals_ignorecase(o._token_impl)

    def get_token_type(self):
        return self._token_impl.get_token_type()

    def get_graphem_tag(self):
        return self._token_impl.get_graphem_tag()

    def get_semantic_tag(self):
        return self._token_impl.get_semantic_tag()

    def get_escaped_data(self):
        return self._token_impl.get_escaped_data()

    def get_length_in_bytes(self):
        return self._token_impl.get_length_in_bytes()

    def __getitem(self, i):
        return self._token_impl[i]

    def __len__(self):
        return len(self._token_impl)

    def __eq__(self, o):
        return self._token_impl == o._token_impl

    def __ne__(self, o):
        return self._token_impl != o._token_impl

    def __lt__(self, o):
        return self._token_impl < o._token_impl

    def __gt__(self, o):
        return self._token_impl > o._token_impl

    def __str__(self):
        return str(self._token_impl)

    def is_upper(self):
        return self._token_impl.is_upper()

    def is_lower(self):
        return self._token_impl.is_lower()

    def is_title(self):
        return self._token_impl.is_title()

    def is_latin(self):
        return self._token_impl.is_latin()

    def is_cyrillic(self):
        return self._token_impl.is_cyrillic()

    def to_JSON(self):
        return dict(
            text=self._token_impl.get_escaped_data(),
            token_type=_to_tag_set(self._token_impl.get_token_type()),
            graphem_tag=_to_tag_set(self._token_impl.get_graphem_tag()),
            semantic_tag=_to_tag_set(self._token_impl.get_semantic_tag()),
        )

    def __repr__(self):
        return str(self.to_JSON())

    def __hash__(self):
        return TokenHasherImpl().hashit(self._token_impl)


class Sentence(object):
    def __init__(self, sentence_impl):
        self._sentence_impl = sentence_impl

    def __str__(self):
        return self._sentence_impl.as_text(False, False)

    def __getitem__(self, index):
        return Token(self._sentence_impl.get_token(index))

    def to_JSON(self):
        return [self.get_token(i).to_JSON() for i in range(len(self))]

    def __len__(self):
        return len(self._sentence_impl)

    def __iter__(self):
        for i in range(len(self)):
            yield self[i]

    def only_words(self):
        return Sentence(SentenceImpl.to_words(self._sentence_impl))

    def is_latin(self):
        return self._sentence_impl.is_latin()

    def is_words_only(self):
        return self._sentence_impl.is_words_only()

    def is_cyrillic(self):
        return self._sentence_impl.is_cyrillic()

    def is_quoted(self):
        return self._sentence_impl.is_quoted()

    def is_starts_correctly(self):
        return self._sentence_impl.is_starts_correctly()

    def is_ends_correctly(self):
        return self._sentence_impl.is_ends_correctly()

    def charactes_count(self):
        return self._sentence_impl.characters_count()

    def words_count(self):
        return self._sentence_impl.words_count()

    def tokens_count(self):
        return len(self)

    def as_text(self, to_lower=False, to_upper=False):
        return self._sentence_impl.as_text(to_lower, to_upper)


class Tokenizer(object):
    def __init__(self, source, buf_size=4096):
        if isinstance(source, str):
            self.strm = CommonInputStreamFromString(source, buf_size)
        elif hasattr(source, "fileno"):
            self.strm = CommonInputStreamFromFD(source.fileno(), buf_size)
        else:
            raise Exception("Unsupported source type")

    def __iter__(self):
        return self

    def __next__(self):
        result = self.strm.read()
        if not result:
            raise StopIteration()
        return Sentence(result)
