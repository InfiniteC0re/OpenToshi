#include "ToshiPCH.h"
#include "PPropertyReader.h"

TOSHI_NAMESPACE_USING

TBOOL PPropertyReader::OnLexerParseError(PPropertyReader* a_this, TFileLexerUTF8* a_lexer, TFileLexerUTF8::ParseError* a_error)
{
    return true;
}

bool PPropertyReader::Open(const Toshi::TCString& a_szFileName)
{
    m_szFileName = a_szFileName;
    if (m_pFile)
    {
        m_pFile->Destroy();
        m_pFile = TNULL;
    }
    if (m_pLexer)
    {
        delete m_pLexer;
        m_pLexer = TNULL;
    }
    m_pFile = TFile::Create(a_szFileName);
    if (m_pFile)
    {
        m_pLexer = new TFileLexerUTF8(m_pFile, 2);
        m_pLexer->SetOutputComments(m_bLoadComments);
        m_oErrorListener.Connect(m_pLexer->GetParseErrorEmitter(), this, OnLexerParseError, 0);
        return TTRUE;
    }
    TCString warn;
    warn.Format("Can't open file", a_szFileName.GetString());
    return TFALSE;
}

bool PPropertyReader::Open(const Toshi::TCString& a_szFileName, Toshi::TFile* a_pFile)
{
    return false;
}

TBOOL PPropertyReader::LoadPropertyBlock(PProperties& a_rProps)
{
    TASSERT(m_pLexer != TNULL);
    return TBOOL();
}
