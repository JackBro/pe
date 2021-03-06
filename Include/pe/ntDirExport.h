/*
 * Copyright (c) 2008-2016, Integrity Project Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the Integrity Project nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE
 */

#ifndef __TBA_STL_PE_NT_DIRECTORY_EXPORT_H
#define __TBA_STL_PE_NT_DIRECTORY_EXPORT_H

/*
 * ntDirExport.h
 *
 * Operation over PE export table.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/list.h"
#include "xStl/data/string.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/stringerStream.h"
#include "pe/ntdir.h"
#include "pe/section.h"
#include "pe/datastruct.h"
#include "pe/ntheader.h"

#define ENTRYPOINT_NAME (XSTL_STRING("EntryPoint"))

/*
 * Forward deceleration for output streams
 */
#ifdef PE_TRACE
class cNtDirExport;
cStringerStream& operator << (cStringerStream& out,
                              const cNtDirExport& object);
#endif // PE_TRACE

/*
 * Handles the ".edata" section - The export table section.
 * Read the section from stream. Provide a set of access utilities and allow
 * to write-back the export section.
 *
 * NOTE: The export table
 */
class cNtDirExport : public cNtDirectory {
public:
    /*
     * Default constructor.
     */
    cNtDirExport();

    /*
     * Read the .edata section from a PE image.
     *
     * header - The NT-header descriptor of the PE file.
     *
     * NOTE: This function extract from the NT-header the location of the export
     *       directory and use the auto-generated 'VirtualMemoryAccesser' in
     *       order to access the export directory and extract the information
     *       from
     *
     * Throw exception if the header doesn't contain a reference for the memory
     * of the PE file.
     */
    cNtDirExport(const cNtHeader& header);

    /*
     * Reads the export table from a stream
     *
     * stream - The stream to read the information from.
     * imageBase - In order to read the name, the relocated offset which the
     *             module is loaded on.
     * entryPoint - The address of the PE entry point, if not 0,
     *              for adding to the end of the export table.
     */
    void read(basicInput& stream,
              addressNumericValue imageBase = 0,
              addressNumericValue entryPoint = 0);

    /*
     * See cNtDirectory::isMyDir
     * Return true on the IMAGE_DIRECTORY_ENTRY_EXPORT
     */
    virtual bool isMyDir(uint directoryTypeIndex);

    /*
     * See cNtDirectory::readDirectory
     * See cNtDirectory::cNtDirectory(const cNtHeader&)
     */
    virtual void readDirectory(const cNtHeader& image,
                               uint directoryTypeIndex = UNKNOWNDIR);

    /*
     * Describe a single function entry within the export directory.
     */
    class cExportEntrie : public cSerializedObject {
    public:
        // Constructor, copy-constructor, operator = will auto-generated by the
        // compiler

        /*
         * Overrides the '>' operator for this class, to be able to sort
         * export entries according to their addresses
         */
        bool operator > (const cNtDirExport::cExportEntrie &other) const;

        // Set to true if the 'm_name' member is in used, otherwise the export
        // function is exported by it's ordinal number
        bool m_isName;
        // The name of the export function
        cString m_name;
        // The virtual (relative) address of the exported function
        remoteAddressNumericValue m_address;
        // The ordinal number
        uint16 m_ordinal;

        // See cSerializedObject::isValid.
        virtual bool isValid() const;
        // See cSerializedObject::serialize.
        virtual void serialize(basicOutput& stream) const;
        // See cSerializedObject::deserialize.
        virtual void deserialize(basicInput& stream);
    };
    typedef cArray<cExportEntrie> ExportTable;

    /*
     * Returns the export table
     */
    const ExportTable& getExportArray() const;

private:
    // Private members

    // Deny copy-constructor and operator =
    cNtDirExport(const cNtDirExport& other);
    cNtDirExport& operator = (const cNtDirExport& other);

    // The friendly trace
    #ifdef PE_TRACE
    friend cStringerStream& operator << (cStringerStream& out,
                                         const cNtDirExport& object);
    #endif //PE_TRACE

    // The IMAGE_EXPORT_DIRECTORY header for the export table
    IMAGE_EXPORT_DIRECTORY m_exportDirectory;
    // A list of all exported functions
    ExportTable m_exportFunctions;
    // The name of the module (this) which export these functions
    cString m_moduleName;
};

#endif // __TBA_STL_PE_NT_DIRECTORY_EXPORT_H
