#include "DAT_Editor.h"
#include "Valkyria_Types.h"
#include "../../Rut/RxStr.h"
#include "../../Rut/RxFile.h"


namespace Valkyria::DAT
{
	bool Unpack(const std::filesystem::path& phPack, const std::filesystem::path& phExtract)
	{
		Rut::RxFile::Binary pack_ifs = { phPack.wstring(), Rut::RIO_READ };

		uint32_t index_size = pack_ifs;

		Rut::RxMem::Auto index_mem(index_size);
		pack_ifs >> index_mem;

		Rut::RxMem::Auto buffer;
		uint32_t entry_count = index_size / sizeof(VAL_Pack_Entry);
		VAL_Pack_Entry* entry_array_ptr = (VAL_Pack_Entry*)index_mem.GetPtr();
		for (size_t ite_entry = 0; ite_entry < entry_count; ite_entry++)
		{
			VAL_Pack_Entry& entry = entry_array_ptr[ite_entry];

			size_t file_size = entry.uiSize;
			size_t file_foa = entry.uiOffset + index_size + sizeof(index_size);
			std::filesystem::path file_save_path = phExtract / Rut::RxStr::ToWCS(entry.aFileName, 932);

			buffer.SetSize(file_size);
			pack_ifs.SetPos(file_foa);
			pack_ifs >> buffer;

			buffer.SaveData(file_save_path.wstring());
		}

		return true;
	}


	bool Pack(const std::filesystem::path& phFolder, const std::filesystem::path& phNewPack)
	{
		std::vector<std::filesystem::path> file_list;
		for (auto& dir_entry : std::filesystem::directory_iterator(phFolder))
		{
			if (dir_entry.is_regular_file() == false) { continue; }
			file_list.push_back(dir_entry.path());
		}
		if (file_list.size() == 0) { throw std::runtime_error("Valkyria::DAT::Pack: Not Find File!"); }

		uint32_t index_size = (uint32_t)(file_list.size() * sizeof(VAL_Pack_Entry));

		Rut::RxFile::Binary pack_ofs = { phNewPack.wstring(), Rut::RIO_WRITE };

		pack_ofs << index_size;

		Rut::RxMem::Auto index_mem(index_size);

		size_t cur_pos = 0;
		VAL_Pack_Entry* entry_array_ptr = (VAL_Pack_Entry*)index_mem.GetPtr();
		for (size_t ite_entry = 0; ite_entry < file_list.size(); ite_entry++)
		{
			VAL_Pack_Entry& entry = entry_array_ptr[ite_entry];
			const std::filesystem::path& file_path = file_list[ite_entry];

			std::string file_name = Rut::RxStr::ToMBCS(file_path.filename().wstring(), 932);
			memset(entry.aFileName, 0x0, sizeof(entry.aFileName));
			memcpy(entry.aFileName, file_name.data(), file_name.size() + 1);

			entry.uiSize = (uint32_t)std::filesystem::file_size(file_path);
			entry.uiOffset = cur_pos;

			cur_pos += entry.uiSize;
		}

		pack_ofs << index_mem;

		Rut::RxMem::Auto buffer;
		for (auto& file_path : file_list)
		{
			buffer.LoadFile(file_path.wstring());
			pack_ofs << buffer;
		}

		return true;
	}
}