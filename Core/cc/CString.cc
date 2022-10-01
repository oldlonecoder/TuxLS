//
// Created by Mobile Klinik on 2022-06-22.
//

#include <TuxLS/Core/CString.h>

namespace Tux
{

[[maybe_unused]] string_view CString::_mDefaultTokenSeparators = "\\%(){}[]`$#@!;,~?^&<>=+-*/:.";


std::string CString::Token::operator()() const
{
    string Str;
    if(mStart == mEnd)
        Str.insert(Str.begin(), mStart, mEnd + 1);
    else
        Str.insert(Str.begin(), mStart, mEnd + 1);

    return Str;
}

std::string CString::Token::operator*() const
{
    string Str;
    if(mStart == mEnd)
        Str.insert(Str.begin(), mStart, mEnd + 1);
    else
        Str.insert(Str.begin(), mStart, mEnd + 1);

    return Str;

}

string CString::Token::Mark() const
{
    string                Str;
    string_view::iterator CStart = mStart - mPosition;

    int                   l  = 1;
    string_view::iterator cc = CStart;
    // localiser le debut de la ligne;
    while(*cc && (cc > CStart) && (*cc != '\n') && (*cc != '\r'))
        --cc;
    // debut de la ligne ou de la source:
    if(cc >= CStart)
    {
        if((*cc == '\n') || (*cc == '\r'))
            ++cc;
        while((cc != mSE) && (*cc != '\n') && (*cc != '\r'))
            Str += *cc++;
    }

    Str += '\n'; ///@todo Encode new line.
    for(int x = 1; x < mCol; x++)
        Str += ' ';
    Str += '^';
    return Str;
}

string CString::Token::Location()
{
    std::ostringstream Str;
    Str << '(' << mLine << ',' << mCol << ')';
    return Str.str();
}


// ------------------------ CString::SPS --------------------------------


CString::SPS::SPS(std::string_view aStr):
mStart(aStr.begin()),
mPos(aStr.begin()),
mStop(aStr.begin())
{

}

/*!
    @note Enleveer le code de trop (suivi du num de ligne et de la col).
*/
bool CString::SPS::Skip()
{
     if(End())
        return false;

    while(isspace(*mPos))
    {
        switch(*mPos)
        {
            case 10:
            {
                if((++mPos) >= mStop)
                    return false;
                if(*mPos == 13)
                    ++mPos;
                ++mLine;
                mCol = 1;
            }
                break;
            case 13:
            {
                if((++mPos) >= mStop)
                    return false;
                if(*mPos == 10)
                    ++mPos;
                ++mLine;
                mCol = 1;
            }
                break;
            case '\t':++mPos;
                ++mCol;
                break;
            default:++mPos;
                ++mCol;
                break;
        }
    }
    return mPos < mStop;

}


bool CString::SPS::End() const
{
    return mPos >= mStop;
}


bool CString::SPS::operator++()
{
    if(mPos >= mStop)
        return false;
    ++mPos;
    ++mCol;
    if(mPos >= mStop)
        return false;
    return Skip();
}
bool CString::SPS::operator++(int)
{
    if(mPos >= mStop)
        return false;
    ++mPos;
    ++mCol;
    if(mPos >= mStop)
        return false;
    return Skip();
}

CString::SPS &CString::SPS::operator>>(CString::Token &aWord)
{
    aWord.mStart    = mPos;
    aWord.mLine     = mLine;
    aWord.mCol      = mCol;
    aWord.mPosition = mIndex = (uint64_t) (mPos - mStart);
    return *this;
}

CString::Iterator CString::ScanTo(CString::Iterator aStart, char c) const
{
    CString::Iterator p = aStart;
    ++p;
    while((p != _mData.end()) && (*p != c))
        ++p;
    return p;
}


/*!
    * @brief break/split/tokenize,etc... the content of this String into pieces.
    * @param CString::Token::List  OUTPUT reference to the 'Tokens array' containter, filled by this method.
    * @param aDelimiters Separators in the form of a string of ascii-8 characters.
    * @param keep_as_word if true (or non-zero), the Separators will be put into the collection as they appear
    * @return number of "Words/tokens" contained into the wcollection.
    * @notice : After several years of experience and experimentations, offset have determined that
    * white-spaces/ctrl or spacing characters are silent and implicit delimiters, in addition to the ones supplied by \c aDelimiters.
    */

std::size_t CString::Tokenize(CString::Token::List &Collection, string_view aDelimiters, bool KeepAsWord) const
{

    auto Crs = CString::SPS(_mData);
    if(_mData.empty())
    {
        std::cout << " --> Contents is Empty!";
        return (std::size_t) 0;
    }
    Crs.Reset(_mData);
    string_view token_separators = aDelimiters.empty() ? CString::_mDefaultTokenSeparators : aDelimiters;
    if(!Crs.Skip())
    {
        //std::cout << " --> Contents Skip is false? (internal?)...\n";
        return (std::size_t) 0;
    }
    Token w;
    Crs >> w;

    while(!Crs.End())
    {
        //if (!wcollection.empty());
        CString::Iterator cc = Crs.mPos;
        if(token_separators.find(*Crs.mPos) != string_view::npos)
        {
            cc = Crs.mPos;
            if(cc > w.mStart)
            {
                --cc;
                Collection.push_back({w.mStart, cc, Crs.mStop, w.mLine, w.mCol, w.mPosition});
                Crs >> w;
                cc = Crs.mPos;
            }

            // '//' as one token instead of having two consecutive '/'
            if((*Crs.mPos == '/') && (*(Crs.mPos + 1) == '/'))
                ++Crs;

            if(KeepAsWord)
            {
                Collection.push_back({w.mStart, Crs.mPos, Crs.mStop, w.mLine, w.mCol, w.mPosition});
            }
            ++Crs;
            //std::cout << "        Iterator eos: " << _Cursor.end() << "\n";
            if(!Crs.End())
                Crs >> w;
            else
            {
                return Collection.size();
            }

        }
        else if((*Crs.mPos == '\'') || (*Crs.mPos == '"'))
        { // Quoted litteral string...
            Crs >> w;
            if(KeepAsWord)
            {
                // Create the three parts of the quoted string: (") + (litteral) + (") ( or ' )
                // So, we save the Token coords anyway.
                Collection.push_back({w.mStart, w.mStart, Crs.mStop, w.mLine, w.mCol, w.mPosition});
            }

            CString::Iterator p = ScanTo(w.mStart + (KeepAsWord ? 0 : 1), *Crs.mPos); // w.B is the starting position, _Cursor.m is the quote delim.
            while(Crs.mPos < p)
                ++Crs; // compute white spaces!!!

            if(KeepAsWord)
            {
                // then push the litteral that is inside the quotes.
                Collection.push_back({w.mStart + 1, p - 1, Crs.mStop, w.mLine, w.mCol, w.mPosition});
                //++_Cursor; // _Cursor now on the closing quote
                Crs >> w; // Litteral is done, update w.
                Collection.push_back({w.mStart, p, Crs.mStop, w.mLine, w.mCol, w.mPosition});
            }
            else
            {
                // Push the entire quote delims surrounding the litteral as the Token.
                Collection.push_back({w.mStart, Crs.mPos, Crs.mStop, w.mLine, w.mCol, w.mPosition});
            }
            if(++Crs)
                Crs >> w;
            else
                return Collection.size();

        }
        else
        {
            cc = Crs.mPos;
            ++cc;
            if(cc == Crs.mStop)
            {
                ++Crs.mPos;
                break;
            }
            if(isspace(*cc))
            {
                if(w.mStart < cc)
                {
                    Collection.push_back({w.mStart, cc - 1, Crs.mStop, w.mLine, w.mCol, w.mPosition});
                    ++Crs;
                }

                if(Crs.Skip())
                {
                    Crs >> w;
                    continue;
                }
                return Collection.size();
            }
            if(!Crs.End())
                ++Crs; // advance offset to the next separator/white space.
        }
    }
    if(Crs.mPos > w.mStart)
        Collection.push_back({w.mStart, Crs.mPos - 1, Crs.mStop, w.mLine, w.mCol, w.mPosition});

    return Collection.size();
}
CString& CString::operator>>(CString::Token::List& Collection)
{
    mConfig.z = Tokenize(mConfig.Collection, mConfig.Delim, mConfig.Keep);
    return *this;
}
CString::CString(string_view aStr):
_mData(aStr)
{

}
CString &CString::operator=(string_view aStr)
{
    _mData = aStr;
    return *this;
}
CString &CString::operator=(const char *aStr)
{
    _mData = aStr;
    return *this;
}
CString::CString(char *aStr): _mData(aStr)
{

}
CString::CString(string_view aStr, string_view Delim, bool KeepDelim):
    _mData(aStr)
{
    mConfig.Delim = Delim;
    mConfig.Keep = KeepDelim;
}


CString::CString(char* aStr, string_view Delim, bool KeepDelim) :
    _mData(aStr)
{
    mConfig.Delim = Delim;
    mConfig.Keep = KeepDelim;
}




CString::CString(const std::string& aStr, string_view Delim, bool KeepDelim) :
    _mData(aStr)
{
    mConfig.Delim = Delim;
    mConfig.Keep = KeepDelim;
}



CString::CString(const char* aStr) : _mData(aStr)
{}


CString::CString(std::string aStr): _mData(aStr.c_str())
{

}


} // Tux